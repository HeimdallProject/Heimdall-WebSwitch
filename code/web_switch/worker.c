#include "../include/worker.h"
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

    //get_log()->d(TAG_WORKER, "request_work");

    // Casts the parameter
    RequestNodePtr node = arg;

    // Gets mutex
    if (pthread_mutex_lock(&node->mutex) != 0) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "read_work");
    }
    //get_log()->d(TAG_WORKER, "prendo mutex request");

    // Asks which host use
    char *host = "192.168.50.3";

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

    //get_log()->d(TAG_WORKER, "creato client");

    // TODO modifies the real request with all attributes @alessio
    // Modifies the simple request with the real new host 
    if (sprintf(node->request->req_host, "%s:80", host) < 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work"));
        *node->worker_status = STATUS_ERROR;
        return NULL; 
    } 

    //get_log()->d(TAG_WORKER, "cambiato ip");

    // Sends request
    throwable = send_http_request(sockfd, node->request);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }

    //get_log()->d(TAG_WORKER, "inviata richiesta");

    // Prepares the http_response
    HTTPResponsePtr http_response = new_http_response();
    node->response = http_response;

    // Receives header into http_response
    throwable = receive_http_response_header(sockfd, http_response);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        *node->worker_status = STATUS_ERROR;
        return NULL;
    }
    //get_log()->d(TAG_WORKER, "ricevuto header, \n%s", http_response->response->header);

    // Sends signal to condition
    if (pthread_cond_signal(&node->condition) != 0) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "send_http_chunks");
    }
    //get_log()->d(TAG_WORKER, "inviato segnale request");

    // Releases mutex
    if (pthread_mutex_unlock(&node->mutex) != 0) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
    }

    //get_log()->d(TAG_WORKER, "rilascio lock request");

    // Receives the response in chunks
    throwable = receive_http_chunks(sockfd, http_response, node->chunk);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        pthread_exit(NULL); // TODO send an error message
    }

    //get_log()->d(TAG_WORKER, "ricevuti chunks");

    // Closes the connection
    throwable = close_connection(sockfd);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        pthread_exit(NULL); // TODO send an error message
    }

    //get_log()->d(TAG_WORKER, "chiusa connessione");

    // Kills the thread TODO is it actually useful?
    // pthread_exit(NULL);
    //get_log()->d(TAG_WORKER, "exiting request");
    return NULL;
}

void *read_work(void *arg) {

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    while (TRUE) {
        // TODO: reading and passing params to HTTPRequest setting the timestamp foreach request in the queue
        worker->watchdog->timestamp_worker = time(NULL);

        // Creates http_request structure
        HTTPRequestPtr http_request = new_http_request();

        // Receives request
        ThrowablePtr throwable = receive_http_request(worker->sockfd, http_request);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            worker->reader_thread_status = STATUS_ERROR;
            return NULL;
        }

        // Puts the http_request into the node
        RequestNodePtr node = init_request_node();
        node->request = http_request;
        node->worker_status = &worker->request_thread_status;

        // Creates the request thread
        int request_creation = pthread_create(&(node->thread_id), NULL, request_work, (void *) node);
        if (request_creation != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "read_work"));
            worker->reader_thread_status = STATUS_ERROR;
            return NULL;
        }

        // Enques the new node
        queue->enqueue(queue, node);

        get_log()->i(TAG_WORKER, "New request enqueued!");
    }
}

void *write_work(void *arg) {

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    while(TRUE) {
        // Gets node
        RequestNodePtr node = queue->get_front(queue);

        if (node != NULL) {
            //get_log()->d(TAG_WORKER, "prendo mutex write");
            // Gets mutex
            if (pthread_mutex_lock(&node->mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work");
            }  

            //get_log()->d(TAG_WORKER, "node->response->response->header %s", node->response->response->header);

            while (node->response->response->header == NULL) {
                //get_log()->d(TAG_WORKER, "aspetto segnale prima write");
                if (pthread_cond_wait(&node->condition, &node->mutex) != 0) {
                    return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work");
                }
                //get_log()->d(TAG_WORKER, "aspetto segnale dopo write");
            }
            //get_log()->d(TAG_WORKER, "get_front %p strlen(node->response->response->header) %d", node, strlen(node->response->response->header));

            //get_log()->d(TAG_WORKER, "sto dentro");

            // Sends the response header
            ThrowablePtr throwable = send_http_response_header(worker->sockfd, node->response);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }
            
            //get_log()->d(TAG_WORKER, "sendato header");

            // Gets the chunk
            ChunkPtr chunk = node->chunk;
            
            //get_log()->d(TAG_WORKER, "chunk %p", chunk);

            // Sends the response chunks
            throwable = send_http_chunks(worker->sockfd, chunk, node->response->response->req_content_len);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
                worker->writer_thread_status = STATUS_ERROR;
                return NULL;
            }

            //get_log()->d(TAG_WORKER, "sendati chunks");

            // Releases mutex
            if (pthread_mutex_unlock(&node->mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
            }
            //get_log()->d(TAG_WORKER, "rilascio lock write");

            // Dequeues the request
            queue->dequeue(queue);

            get_log()->i(TAG_WORKER, "Request dequeued!");
        }
    }
    
    return NULL;
}

void start_worker(int fd) {

    // Initializes worker
    WorkerPtr worker = new_worker();
    // Sets socket to worker
    worker->sockfd = fd;

    // Initializes queue
    RequestQueuePtr queue = init_request_queue();
    worker->requests_queue = queue;

    // Initializes watchdog
    ThrowablePtr throwable = detach_watchdog(worker->watchdog);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    // ?
    int watch_creation = pthread_create(&(worker->watch_thread), NULL, enable_watchdog, (void *) worker->watchdog);
    if (watch_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // Initializes writer
    int w_creation = pthread_create(&(worker->writer_thread), NULL, write_work, (void *) worker);
    if (w_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // Initializes reader
    int r_creation = pthread_create(&(worker->reader_thread), NULL, read_work, (void *) worker);
    if (r_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // Detachs all the threads - no join, just condition waiting
    // (dev: the 'pthread_detach' does not change errno value, be aware!)
    if (pthread_detach(worker->watch_thread) != 0 || pthread_detach(worker->writer_thread) != 0 || pthread_detach(worker->reader_thread) != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(ESRCH), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // Enters in mutex-condition loop
    int awaiting;
    int mtx_lock_unlock = pthread_mutex_lock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    while (worker->worker_await_flag == WATCH_TOWER) {
        awaiting = pthread_cond_wait(&worker->await_cond, &worker->await_mtx);
        if (awaiting != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
            exit(EXIT_FAILURE);
        } else {
            //get_log()->d(TAG_WORKER, "awaiting");
            break;
        }
    }
    //get_log()->d(TAG_WORKER, "dopo");

    mtx_lock_unlock = pthread_mutex_unlock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }
    //get_log()->d(TAG_WORKER, "lascio mutex");

    // Checks for status of the threads and then exiting
    if (worker->watchdog->status == STATUS_ERROR || worker->writer_thread_status  == STATUS_ERROR || worker->reader_thread_status  == STATUS_ERROR || worker->request_thread_status  == STATUS_ERROR) {
        //get_log()->d(TAG_WORKER, "status error");
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    } else {
        //get_log()->d(TAG_WORKER, "status ok");
        return ;
    }
}