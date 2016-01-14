#include "../include/worker.h"

static pthread_mutex_t mtx_thr_request = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_thr_request = PTHREAD_COND_INITIALIZER;

static int thread_req = 0;

/*
 * ---------------------------------------------------------------------------
 * Message structure for messages in the shared segment
 * ---------------------------------------------------------------------------
 */

// struct shared_data {
//     int conc_connections;
// };

// /*
//  * ---------------------------------------------------------------------------
//  * Global variable for shared memory
//  * ---------------------------------------------------------------------------
//  */
// static int shmfd;                                               /* shared memory file descriptor */
// static int shared_seg_size = (1 * sizeof(struct shared_data));  /* want shared segment capable of storing 1 message */
// static struct shared_data *shared_msg;                          /* the shared segment, and head of the messages list */
// static sem_t * sem_id;                                          /* sem used for access in shared memory */

// ThrowablePtr set_shm2(){
    
//     // creating the shared memory object
//     shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
//     if (shmfd < 0){
//         return get_throwable()->create(STATUS_ERROR, "shm_open", "set_shm");
//     }

//     // adjusting mapped file size
//     if(ftruncate(shmfd, shared_seg_size) == -1){
//         return get_throwable()->create(STATUS_ERROR, "ftruncate", "set_shm");
//     }

//     // Semaphore open
//     sem_id = sem_open(SHMOBJ_SEM, O_CREAT, S_IRUSR | S_IWUSR, 1);
//     if(sem_id == SEM_FAILED){
//         return get_throwable()->create(STATUS_ERROR, "sem_open", "set_shm");
//     }

//     // requesting the shared segment mmap()
//     shared_msg = (struct shared_data *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
//     if (shared_msg == NULL){
//         return get_throwable()->create(STATUS_ERROR, "mmap", "set_shm");
//     }

//     return get_throwable()->create(STATUS_OK, NULL, "set_shm()");;
// } 

// ThrowablePtr update_shm2(){

//     if(sem_wait(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_wait", "update_shm");
//     }

//     get_log()->d(TAG_WORKER, "Open sem for update shared memory");

//     shared_msg->conc_connections = shared_msg->conc_connections-1;

//     get_log()->d(TAG_WORKER, "The total of concurrent connections are %d",shared_msg->conc_connections);

//     if(sem_post(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_post", "update_shm");
//     }

//     get_log()->d(TAG_WORKER, "Sem closed");

//     return get_throwable()->create(STATUS_OK, NULL, "update_shm()");;
// }

/*
 *  See .h for more information.
 */
WorkerPtr new_worker() {

    WorkerPtr worker = malloc(sizeof(Worker));
    if (worker == NULL) {
        get_log()->e(TAG_WORKER, "Memory allocation error in new_worker!");
        exit(EXIT_FAILURE);
    }

    // Sets new watchdog
    worker->watchdog = new_watchdog();

    // Initializes condition and mutex
    if (pthread_cond_init(&worker->await_cond, NULL) != 0) {
        get_log()->e(TAG_WORKER, "Error in pthread_cond_init!");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&worker->await_mtx, NULL) != 0) {
        get_log()->e(TAG_WORKER, "Error in pthread_cond_init!");
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

    // Casts the parameter
    RequestNodePtr node = arg;

    // Gets mutex
    if (pthread_mutex_lock(&node->mutex) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // Asks which host use
    char *host;
    host = get_scheduler()->get_server(get_scheduler()->rrobin)->ip;
    get_log()->d(TAG_WORKER, "SERVER : %s", host);

    // TODO create the function @alessio
    // ThrowablePtr throwable = get_host(host);
    // if (throwable->is_an_error(throwable)) {
    //     get_log()->t(throwable);
    //     pthread_exit(NULL); // TODO send an error message
    // }

    // Creates a new client
    int sockfd;
    ThrowablePtr throwable = create_client_socket(TCP, host, 80, &sockfd);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // TODO modifies the real request with all attributes @alessio
    // Modifies the simple request with the real new host 

    if (asprintf(&(node->request->req_host), "%s:80", host) < 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        return NULL; 
    }

    get_log()->d(TAG_WORKER, "HOST: %s", node->request->req_host);

    // Sends request
    throwable = send_http_request(sockfd, node->request);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // Receives header into http_response
    throwable = receive_http_response_header(sockfd, node->response);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    // Sends signal to condition
    if (pthread_cond_signal(&node->condition) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
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

    // Kills the thread TODO is it actually useful?
    //pthread_exit(NULL);

    return NULL;
}

void *read_work(void *arg) {

    get_log()->d(TAG_WATCHDOG, "read_work");

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    while (TRUE) {

        // TODO: reading and passing params to HTTPRequest setting the timestamp foreach request in the queue
        worker->watchdog->timestamp_worker = time(NULL);

        // Creates the node
        RequestNodePtr node = init_request_node();
        if (node == NULL) {
            get_log()->e(TAG_WORKER, "Malloc not fatt");
            worker->reader_thread_status = STATUS_ERROR;
            return NULL;
        }
        node->worker_status = &worker->request_thread_status;

        // Receives request
        ThrowablePtr throwable = receive_http_request(worker->sockfd, node->request);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            worker->reader_thread_status = STATUS_ERROR;
            return NULL;
        }

        // Enques the new node
        queue->enqueue(queue, node);

        get_log()->i(TAG_WORKER, "New request enqueued!");

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

        // Waits until wrote is < 2 with condition
        while (thread_req >= 2) { 
            if (pthread_cond_wait(&cond_thr_request, &mtx_thr_request) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
            }
        }

        thread_req++;

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

    get_log()->d(TAG_WATCHDOG, "write_work");

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    while(TRUE) {

        // Gets node
        RequestNodePtr node = queue->get_front(queue);

        if (node != NULL) {
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

            // Releases mutex
            if (pthread_mutex_unlock(&node->mutex) != 0) {
                get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }

            get_log()->i(TAG_WORKER, "Request dequeued!");

            // Dequeues the request and it destroys that
            node = queue->dequeue(queue);
            node->destroy(node);
            
            // Gets mutex
            if (pthread_mutex_lock(&mtx_thr_request) != 0) {
                get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }  

            while (thread_req < 2) {
                if (pthread_cond_wait(&cond_thr_request, &mtx_thr_request) != 0) {
                    get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work"));
                    worker->writer_thread_status = STATUS_ERROR;
                    return NULL;
                }
            }

            // Set condition request for perfom something on the pool
            thread_req--;

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
    
    return NULL;
}

void handler_sig(int signo) {

    get_log()->i(TAG_WORKER, "\n\n\n\n\n\n\n\n BOOOOM!\nBOOOOM!\nBOOOOM!\nBOOOOM!\nBOOOOM!\nBOOOOM!\nBOOOOM!\nBOOOOM!\n \n\n\n\n\n %d", signo);

    //update_shm2();

    get_log()->d(TAG_WORKER, "End job (error2), Bye bye %ld", (long)getpid());

    exit(EXIT_FAILURE);
}

void start_worker(int fd) {

    sigset_t sigset, oldset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &sigset, &oldset);

    // Initializes worker
    WorkerPtr worker = new_worker();
    // Sets socket to worker
    worker->sockfd = fd;

    // Create shared memory when we keep track of the total for concurrent connection
    /*ThrowablePtr throwable = set_shm2();
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    } */

    // Initializes queue
    RequestQueuePtr queue = init_request_queue();
    worker->requests_queue = queue;

    // Initializes watchdog
    ThrowablePtr throwable = detach_watchdog(worker->watchdog);
    get_log()->i(TAG_WORKER, "DETACCIATO\n\n!");
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    // ?
    int watch_creation = pthread_create(&(worker->watch_thread), NULL, enable_watchdog, (void *) worker->watchdog);
    get_log()->d(TAG_WORKER, "W creation %d", watch_creation);
    if (watch_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_1"));
        exit(EXIT_FAILURE);
    }

    // Initializes writer
    int w_creation = pthread_create(&(worker->writer_thread), NULL, write_work, (void *) worker);
    if (w_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_2"));
        exit(EXIT_FAILURE);
    }

    // Initializes reader
    int r_creation = pthread_create(&(worker->reader_thread), NULL, read_work, (void *) worker);
    if (r_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_3"));
        exit(EXIT_FAILURE);
    }

    // Detachs all the threads - no join, just condition waiting
    // (dev: the 'pthread_detach' does not change errno value, be aware!)
    if (pthread_detach(worker->watch_thread) != 0 || pthread_detach(worker->writer_thread) != 0 || pthread_detach(worker->reader_thread) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(ESRCH), "start_worker_4"));
        exit(EXIT_FAILURE);
    }

    // Install the signal handler for SIGPIPE.
    struct sigaction s;
    s.sa_handler = handler_sig;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    sigaction(SIGPIPE, &s, NULL);

    // Restore the old signal mask only for this thread.
    pthread_sigmask(SIG_SETMASK, &oldset, NULL);

    // Enters in mutex-condition loop
    int awaiting;
    int mtx_lock_unlock = pthread_mutex_lock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_5"));
        exit(EXIT_FAILURE);
    }

    while (worker->worker_await_flag == WATCH_TOWER) {

        get_log()->d(TAG_WORKER, "Dormo su cond %p - %p \n\n", &worker->await_cond, &worker->await_mtx);

        awaiting = pthread_cond_wait(&worker->await_cond, &worker->await_mtx);
        if (awaiting != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_6"));
            exit(EXIT_FAILURE);
        }
    }

    get_log()->d(TAG_WORKER, "dopo");

    mtx_lock_unlock = pthread_mutex_unlock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker_7"));
        exit(EXIT_FAILURE);
    }
    //get_log()->d(TAG_WORKER, "lascio mutex");

    // Checks for status of the threads and then exiting
    if (worker->watchdog->status == STATUS_ERROR || worker->writer_thread_status  == STATUS_ERROR || worker->reader_thread_status  == STATUS_ERROR || worker->request_thread_status  == STATUS_ERROR) {
        
        /*throwable = update_shm2();
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            exit(EXIT_FAILURE);
        }*/

        get_log()->d(TAG_WORKER, "End job (error), Bye bye %ld", (long)getpid());

        exit(EXIT_FAILURE);

    } else {

        /*throwable = update_shm2();
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            exit(EXIT_FAILURE);
        }*/

        get_log()->d(TAG_WORKER, "End job, Bye bye %ld", (long)getpid());
        exit(EXIT_SUCCESS);
    }
}