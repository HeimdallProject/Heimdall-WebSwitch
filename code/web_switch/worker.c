#include "../include/worker.h"
#include <syscall.h>

static pthread_mutex_t mtx_thr_request;
static pthread_cond_t cond_thr_request;

static int max_thr_request = 0;
static int request_counter = 0;

THPSharedMemPtr worker_pool;
sem_t *sem;

/*
 *  See .h for more information.
 */
WorkerPtr new_worker() {

    get_log()->d(TAG_WORKER, "%ld - new_worker", (long) getpid());

    WorkerPtr worker = malloc(sizeof(Worker));
    if (worker == NULL) {
        get_log()->e(TAG_WORKER, "Memory allocation error in new_worker!");
        exit(EXIT_FAILURE);
    }

    // Sets new watchdog
    worker->watchdog = new_watchdog();

    // Initializes queue
    RequestQueuePtr queue = init_request_queue();
    worker->requests_queue = queue;

    // Initializes condition and mutex
    if (pthread_cond_init(&worker->await_cond, NULL) != 0) {
        get_log()->e(TAG_WORKER, "Error in pthread_cond_init!");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&worker->await_mtx, NULL) != 0) {
        get_log()->e(TAG_WORKER, "Error in pthread_mutex_init!");
        exit(EXIT_FAILURE);
    }

    worker->worker_await_flag           = WATCH_TOWER;
    worker->reader_thread_status        = STATUS_OK;
    worker->writer_thread_status        = STATUS_OK;
    worker->request_thread_status       = STATUS_OK;

    worker->watchdog->status            = STATUS_OK;

    worker->watchdog->worker_await_cond = &worker->await_cond;
    worker->watchdog->worker_await_flag = &worker->worker_await_flag;

    return worker;
}

void *request_work(void *arg) {

    int sid = syscall(SYS_gettid);
    get_log()->d(TAG_WORKER, "request_work %d", sid);

    //get_log()->d(TAG_WORKER, "%ld - request_work", (long) getpid());

    // Casts the parameter
    RequestNodePtr node = arg;

    // Gets mutex
    if (pthread_mutex_lock(&node->mutex) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // Using shared memory to retrieve the host
    ConfigPtr config = get_config();

    int number_of_worker = 0;
    ThrowablePtr throwable = str_to_int(config->pre_fork, &number_of_worker);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
    }

    // Asks which host use
    ServerPtr remote = NULL;
    
    // retrieving remote host from the shared memory

    if(sem_wait(sem) == -1){
        get_log()->e(TAG_WORKER, "Error in sem_wait - start_worker");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i < number_of_worker; ++i){
        if (worker_pool->worker_array[i] == (long)getpid()){
            remote = &(worker_pool->worker_server[i]);
            break;
        }
    }

    if(sem_post(sem) == -1){
        get_log()->e(TAG_WORKER, "Error in sem_wait - start_worker");
        exit(EXIT_FAILURE);
    }

    // checking for remote host status
    if (remote->status == SERVER_STATUS_BROKEN) {
        *node->worker_status = STATUS_ERROR;

        // if we get the server status as broken
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
            exit(EXIT_FAILURE);
        }

        return NULL;

    }
    // checking for errors in passing the remote host...
    // ... and proceeding if it is all fine!
    char *host = remote->ip;
    if (host == NULL) {
        *node->worker_status = STATUS_ERROR;

        // if we get error in server params
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
            exit(EXIT_FAILURE);
        }

        return NULL;
    }

    get_log()->d(TAG_WORKER, "========= HOST POINTER : %p", host);
    //get_log()->d(TAG_WORKER, "========= HOST: %s", host);

    // Creates a new client
    int sockfd;
    throwable = create_client_socket(TCP, host, 80, &sockfd);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
            
        // Releases mutex
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
            exit(EXIT_FAILURE);
        }

        return NULL;
    }

    // Logging - request
    HTTPRequestPtr request = node->request;
    get_log()->r(RQST, (void *)request, host, getpid());

    get_log()->d(TAG_WORKER, "io Dott. %ld del %ld Faccio richiesta a %s su socket: %d", (long) pthread_self(), (long) getpid(), node->request->req_host, sockfd);

    if (asprintf(&(node->request->req_host), "%s:80", host) < 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        
        // Releases mutex
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
        }

        return NULL;
    }

    // Sends request
    throwable = send_http_request(sockfd, node->request);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
    
        // Releases mutex
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
        }

        return NULL;
    }

    // Receives header into http_response
    throwable = receive_http_response_header(sockfd, node->response);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        
        // Releases mutex
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
        }

        return NULL;
    }

    // Logging - response
    HTTPResponsePtr response = node->response;
    get_log()->r(RESP, (void *)response, host, getpid());


    // Sends signal to condition
    if (pthread_cond_signal(&node->condition) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        
        // Releases mutex
        if (pthread_mutex_unlock(&node->mutex) != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
            *node->worker_status = STATUS_ERROR;
        }

        return NULL;
    }

    // Releases mutex
    if (pthread_mutex_unlock(&node->mutex) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // Receives the response in chunks
    throwable = receive_http_chunks(sockfd, node->response, node->chunk);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // Closes the connection
    throwable = close_connection(sockfd);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    pthread_exit(NULL);
}

void *read_work(void *arg) {

    int sid = syscall(SYS_gettid);
    get_log()->d(TAG_WORKER, "read_work %d", sid);

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    get_log()->d(TAG_WORKER, "%ld - read_work socket %d", (long) getpid(), worker->sockfd);

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    ConfigPtr config = get_config();
    
    int max_thread_pchild = 0;
    ThrowablePtr throwable = str_to_int(config->max_thread_pchild, &max_thread_pchild);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
    }

    while (TRUE) {

        // Waits
        while (max_thr_request > max_thread_pchild) {
            if (pthread_cond_wait(&cond_thr_request, &mtx_thr_request) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
            }
        }
        
        // Updates timer
        worker->watchdog->timestamp_worker = time(NULL);

        // Creates the node
        RequestNodePtr node = init_request_node();
        if (node == NULL) {
            get_log()->e(TAG_WORKER, "Malloc error in init_request_node");
            worker->reader_thread_status = STATUS_ERROR;
            return NULL;
        }
        node->worker_status = &worker->request_thread_status;

        get_log()->i(TAG_WORKER, "%ld - request_counter = %d - queue_size = %d", (long) getpid(), request_counter, queue->get_size(queue));

        // Enques the new node
        queue->enqueue(queue, node);
        
        // Receives request
        ThrowablePtr throwable = receive_http_request(worker->sockfd, node->request);
        if (throwable->is_an_error(throwable)) {
            
            get_log()->t(throwable);
            worker->reader_thread_status = STATUS_ERROR;
            
            // if we get some error on cliebt socket 
            worker->worker_await_flag = WATCH_OVER;
            pthread_cond_signal(&worker->await_cond);
            
            pthread_exit(NULL);
        }

        request_counter++;

        get_log()->i(TAG_WORKER, "%ld - New request enqueued!", (long) getpid());

        // Creates the request thread
        int request_creation = pthread_create(&(node->thread_id), NULL, request_work, (void *) node);
        if (request_creation != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "read_work"));
            worker->reader_thread_status = STATUS_ERROR;
            return NULL;
        }

       // Gets mutex
       if (pthread_mutex_lock(&mtx_thr_request) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
       }

       max_thr_request++;

        // Sends signal to condition
        if (pthread_cond_signal(&cond_thr_request) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
        }

        // Releases mutex
        if (pthread_mutex_unlock(&mtx_thr_request) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
        }
    }
}

void *write_work(void *arg) {

    int sid = syscall(SYS_gettid);
    get_log()->d(TAG_WORKER, "write_work %d", sid);

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;
    get_log()->d(TAG_WORKER, "%ld - write_work socket %d", (long) getpid(), worker->sockfd);

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    while(TRUE) {

        // Gets node
        RequestNodePtr node = queue->get_front(queue);

        if (node != NULL) {
            get_log()->d(TAG_WORKER, "%ld - write_work socket %d, node!=NULL", (long) getpid(), worker->sockfd);

            // Gets mutex
            if (pthread_mutex_lock(&node->mutex) != 0) {
                get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }  

            while (node->response->response->header == NULL) {
                if (pthread_cond_wait(&node->condition, &node->mutex) != 0) {
                    get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                    worker->writer_thread_status = STATUS_ERROR;
                    return NULL;
                }
            }

            // Sends the response header
            ThrowablePtr throwable = send_http_response_header(worker->sockfd, node->response);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
                worker->writer_thread_status = STATUS_ERROR;
                
                // unlock if error
                if (pthread_mutex_unlock(&node->mutex) != 0) {
                    get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                }

                return NULL;
            }
            
            // Releases mutex
            if (pthread_mutex_unlock(&node->mutex) != 0) {
                get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }

            // Gets the chunk
            ChunkPtr chunk = node->chunk;
            
            // Sends the response chunks
            throwable = send_http_chunks(worker->sockfd, chunk, node->response->response->req_content_len);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }

            // Dequeues the request and it destroys that
            node = queue->dequeue(queue);
            node->destroy(node);

            get_log()->i(TAG_WORKER, "%ld - Request dequeued!", (long) getpid());

        }
    }
    
    return NULL;
}

void start_worker() {

    int sid = syscall(SYS_gettid);
    get_log()->d(TAG_WORKER, "start_worker %d", sid);

    //get_log()->d(TAG_WORKER, "%ld - start_worker", (long) getpid());

    ConfigPtr config = get_config();
    
    int number_of_worker = 0;
    ThrowablePtr throwable = str_to_int(config->pre_fork, &number_of_worker);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
    }

    worker_pool = get_shm(WRK_SHM_PATH);
    if (worker_pool == NULL){
        get_log()->e(TAG_WORKER, "Error in get_shm2 - start_worker");
        exit(EXIT_FAILURE);
    }

    // update worker status
    sem = sem_open(WRK_SEM_PATH, 0);

    while(TRUE) {

        get_log()->i(TAG_WORKER, "%ld in wait for fd", (long) getpid());

        int file_descriptor = 0;

        // open unix socket for receice file_descriptor from main
        throwable = receive_fd(&file_descriptor, getpid());
        if (throwable->is_an_error(throwable)) {
            get_log()->e(TAG_WORKER, "Error in receive_fd()");
            get_log()->t(throwable);
            exit(EXIT_SUCCESS);
        }

        get_log()->i(TAG_WORKER, "%ld fd received: %d", (long) getpid(), file_descriptor);

        // Initializes worker
        WorkerPtr worker = new_worker();
        worker->sockfd = file_descriptor;

        // Initializes watchdog
        throwable = detach_watchdog(worker->watchdog);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            exit(EXIT_FAILURE);
        }


        // Initializes request condition
        if (pthread_cond_init(&cond_thr_request, NULL) != 0) {
            get_log()->e(TAG_WORKER, get_error_by_errno(errno));
            exit(EXIT_FAILURE);    
        }

        // Initializes request mutex
        if (pthread_mutex_init(&mtx_thr_request, NULL) != 0) {
            get_log()->e(TAG_WORKER, get_error_by_errno(errno));
            exit(EXIT_FAILURE);
        }


        // Initializes Whatchdog
        int watch_creation = pthread_create(&(worker->watch_thread), NULL, enable_watchdog, (void *) worker->watchdog);
        if (watch_creation != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, "pthread_create watch_creation", "start_worker_1"));
            exit(EXIT_FAILURE);
        }

        // Initializes writer
        int w_creation = pthread_create(&(worker->writer_thread), NULL, write_work, (void *) worker);
        if (w_creation != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, "pthread_create w_creation", "start_worker_2"));
            exit(EXIT_FAILURE);
        }

        // Initializes reader
        int r_creation = pthread_create(&(worker->reader_thread), NULL, read_work, (void *) worker);
        if (r_creation != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, "pthread_create r_creation", "start_worker_3"));
            exit(EXIT_FAILURE);
        }

        // Enters in mutex-condition loop
        int awaiting;
        int mtx_lock_unlock = pthread_mutex_lock(&worker->await_mtx);
        if (mtx_lock_unlock != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_5"));
            exit(EXIT_FAILURE);
        }

        while (worker->worker_await_flag == WATCH_TOWER) {

            awaiting = pthread_cond_wait(&worker->await_cond, &worker->await_mtx);
            if (awaiting != 0) {
                get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_6"));
                exit(EXIT_FAILURE);
            }
        }

        mtx_lock_unlock = pthread_mutex_unlock(&worker->await_mtx);
        if (mtx_lock_unlock != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_7"));
            exit(EXIT_FAILURE);
        }

        // Checks for status of the threads and then exiting
        if (worker->watchdog->status == STATUS_ERROR) {
            get_log()->e(TAG_WORKER, "End connection job (error watchdog) - %ld", (long) getpid());
        } else if (worker->writer_thread_status  == STATUS_ERROR) {
            get_log()->e(TAG_WORKER, "End connection job (error writer_thread) - %ld", (long) getpid());
        } else if (worker->reader_thread_status  == STATUS_ERROR) {
            get_log()->e(TAG_WORKER, "End connection job (error reader_thread) - %ld", (long) getpid());    
        } else if (worker->request_thread_status  == STATUS_ERROR) {
            get_log()->e(TAG_WORKER, "End connection job (error request_thread) - %ld", (long) getpid());
        } else {
            get_log()->d(TAG_WORKER, "End connection job (all ok) - %ld", (long) getpid());
        }

        get_log()->d(TAG_WORKER, "%ld -> CLOSE %d", (long)getpid(), worker->sockfd);

        // close socket connection
        throwable = close_connection(worker->sockfd);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
        }

        char *pathname;
        // TODO da aggiungere in config!
        if (asprintf(&pathname, "%s_%ld", config->sockets_path, (long) getpid()) < 0) {
            get_log()->e(TAG_WORKER, "asprintf in start_worker");
        }

        /* Create socket bound to well-known address */
        if (remove(pathname) == -1 && errno != ENOENT) {
            get_log()->e(TAG_WORKER, "remove in start_worker");
        }

        // Destroy watchdog cond and mutex
        if (pthread_cond_destroy(&worker->await_cond) != 0) {
            get_log()->e(TAG_WORKER, "Error in pthread_cond_destroy");
        }
        if (pthread_mutex_destroy(&worker->await_mtx) != 0) {
            get_log()->e(TAG_WORKER, "Error in pthread_mutex_destroy");
        }

        // Reset counter thread request
        max_thr_request = 0;

        // Destroy max request cond and mutex
        if (pthread_cond_destroy(&cond_thr_request) != 0) {
            get_log()->e(TAG_WORKER, "Error in pthread_cond_destroy");
        }
        if (pthread_mutex_destroy(&mtx_thr_request) != 0) {
            get_log()->e(TAG_WORKER, "Error in pthread_mutex_destroy");
        }

        pthread_cancel(worker->watch_thread);
        pthread_cancel(worker->writer_thread);
        pthread_cancel(worker->reader_thread);

        free(worker);

        if(sem_wait(sem) == -1){
            get_log()->e(TAG_WORKER, "Error in sem_wait - start_worker");
            exit(EXIT_FAILURE);
        }

        int i;
        for (i = 0; i < number_of_worker; ++i){
            if (worker_pool->worker_array[i] == (long)getpid()){
                worker_pool->worker_busy[i] = 0;
                get_log()->i(TAG_WORKER, "Worker %ld end job", (long)getpid());
                break;
            }
        }

        // Print array
        for (i = 0; i < number_of_worker; ++i){
            get_log()->i(TAG_WORKER, "Worker %ld in position %d available %d - usage %d", 
                                    (long)worker_pool->worker_array[i], 
                                    i, 
                                    worker_pool->worker_busy[i],
                                    worker_pool->worker_counter[i]);
        }

        if(sem_post(sem) == -1){
            get_log()->e(TAG_WORKER, "Error in sem_wait - start_worker");
            exit(EXIT_FAILURE);
        }

        get_log()->d(TAG_WORKER, "Restart connection job - %ld", (long) getpid());
    }
}








