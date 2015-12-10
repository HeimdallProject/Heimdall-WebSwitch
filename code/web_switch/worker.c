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

    // watchdog struct allocation
    worker->watchdog = malloc(sizeof(Watchdog));
    if (worker->watchdog == NULL) {
        get_log()->e(TAG_WORKER, "Memory allocation error in new_worker!");
        exit(EXIT_FAILURE);
    }

    // initializing condition and mutex
    if (pthread_cond_init(&worker->await_cond, NULL) != 0) {
        get_log()->e(TAG_WORKER, "pthread_cond_init");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&worker->await_mtx, NULL) != 0) {
        get_log()->e(TAG_WORKER, "pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    // presetting threads attributes
    worker->worker_await_flag           = WATCH_TOWER;

    worker->reader_thread_status        = STATUS_OK;
    worker->writer_thread_status        = STATUS_OK;
    worker->watchdog->status            = STATUS_OK;

    worker->watchdog->worker_await_cond = &worker->await_cond;
    worker->watchdog->worker_await_flag = &worker->worker_await_flag;

    return worker;
}

void *request_work(void *arg) {

    // Casts the parameter
    RequestNodePtr node = arg;

    //TODO  il threadRequest dovrà richiedere al componente Scheduler su quale server inoltrare la richiesta.
    char *ip = "192.168.50.3"; 
    //Come descritto in cima al documento lo Scheduler sarà pronto a fornire il server da contattare, ricevuto 
    //quindi l’indirizzo IP della macchina il threadRequest invierà una richiesta HTTP al server selezionato e 
    //resterà in attesa della risposta.
    
    // Creates a new client
    int sockfd;
    ThrowablePtr throwable = create_client_socket(TCP, ip, 80, &sockfd);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "request_work");
    }

    // Modifies the simple request with the real new host 
    char *host = (char *) malloc(sizeof(ip) + 1 + 5);
    if (host == NULL) {
        get_throwable()->create(STATUS_ERROR, "Allocation error", "request_work");
        pthread_exit(NULL); // TODO send an error message
    }

    if (sprintf(node->request->req_host, "%s:80", ip) < 0) {
        get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work");
        pthread_exit(NULL); // TODO send an error message    
    } 

    // Generates the simple request
    char *request_message;
    throwable = node->request->make_simple_request(node->request, &request_message);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "request_work");
    }

    // Sends request
    throwable = send_request(&sockfd, request_message);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "request_work");
    }

    // Prepares the http_response
    HTTPResponsePtr http_response = new_http_response();

    // Sets the response into the node
    node->response = http_response;

    // Prepares in order to receive the header
    char *header = (char *) malloc(sizeof(char) * 1024);
    if (header == NULL) {
        get_throwable()->create(STATUS_ERROR, "Allocation error", "request_work");
        pthread_exit(NULL); // TODO send an error message
    }

    // Receives the header
    throwable = receive_http_header(&sockfd, header);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        pthread_exit(NULL); // TODO send an error message
    }

    get_log()->i(TAG_WORKER, "%s", header);
    http_response->response->header = (char *) malloc(sizeof(char) * strlen(header));
    if (http_response->response->header == NULL) {
        get_throwable()->create(STATUS_ERROR, "Allocation error", "request_work");
        pthread_exit(NULL); // TODO send an error message
    }
    if (strcpy(http_response->response->header, header) != http_response->response->header) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "request_work");
    }

    // // Gets the header lenght
    // int header_length = strlen(header);

    // Sets and it parses header
    throwable = http_response->get_response_head(http_response, header); // TODO ale fatte na copia della stringa che rovini...
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        pthread_exit(NULL); // TODO send an error message
    }

    // Gets the response lenght
    int response_lenght;
    throwable = str_to_int(http_response->response->req_content_len, &response_lenght);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        pthread_exit(NULL); // TODO send an error message
    }

/*    // Parses the response
    http_response->get_http_response(http_response, response);*/

    // Prepares in order to receive the response
    http_response->http_response_body = (char *) malloc(sizeof(char) * 1024);
    if (http_response->http_response_body == NULL) {
        get_throwable()->create(STATUS_ERROR, "Allocation error", "request_work");
        pthread_exit(NULL); // TODO send an error message
    }

    // Receives the response
    throwable = receive_http_body(&sockfd, http_response->http_response_body, response_lenght, &(node->mutex), &(node->wrote), &(node->dimen));
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        pthread_exit(NULL); // TODO send an error message
    }

    close_connection(sockfd);

    // Kills the thread
    pthread_exit(NULL);

    return NULL;
}

void *read_work(void *arg) {

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    while (TRUE) {
        // TODO: reading and passing params to HTTPRequest setting the timestamp foreach request in the QUEUE
        worker->watchdog->timestamp_worker = time(NULL);

        /*// TODO creates something like this
        ThrowablePtr receive_http_request(int sockfd, HTTPRequestPtr http_request) {
            ThrowablePtr throwable = receive_http_header(sockfd, http_request->request);
            if (throwable->is_an_error(throwable)) {

            }
            
            throwable = http_request->read_request(http_request);
            if (throwable->is_an_error(throwable)) {

            }

            return 
        }*/

        // Prepares buffer in order to receive the response
        char *header = (char *) malloc(sizeof(char) * 1024);
        if (header == NULL) {
            get_log()->e(TAG_WORKER, "Memory allocation error in read_work!");
            pthread_exit(NULL); // TODO send an error message
        }

        // Receives the response
        ThrowablePtr throwable = receive_http_header(&(worker->sockfd), header);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            pthread_exit(NULL); // TODO send an error message
        }

        // Closes the connection
        //TODO maybe not in order to have persistent connection
        //close_connection(worker->sockfd);

        // Creates http_request structure
        HTTPRequestPtr http_request = new_http_request();

        // Parses the request
        throwable = http_request->read_headers(header, http_request, RQST);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            pthread_exit(NULL); // TODO send an error message
        }

        // Puts the http_request into the node and after it puts the node into the queue
        RequestNodePtr node = init_request_node();
        node->request = http_request;
        queue->enqueue(queue, node);

        get_log()->d(TAG_WORKER, "read_work queue %s", queue->to_string(queue));

        // Creates the request thread
        int request_creation = pthread_create(&(node->thread_id), NULL, request_work, (void *) node);
        if (request_creation != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "read_work"));
            pthread_exit(NULL); // TODO send an error message
        }

        get_log()->i(TAG_WORKER, "New request enqueued!");
    }
}

void *write_work(void *arg) {

    // Casts the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    // Gets queue
    RequestQueuePtr queue = worker->requests_queue;

    int response_send_lenght = 0;

    int header_write = FALSE;

    while(TRUE) {
        usleep(100000); 
        get_log()->d(TAG_WORKER, "write_work prendo il nodo");

        RequestNodePtr node = queue->get_front(queue);
        get_log()->d(TAG_WORKER, "write_work nodo %p", node);


        if (node != NULL) {
            get_log()->d(TAG_WORKER, "write_work lock mutex");
            if (pthread_mutex_lock(&(node->mutex)) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work");
            }

            if (header_write == FALSE) {
                get_log()->d(TAG_WORKER, "write_work header_write false");

                ThrowablePtr throwable = send_request(&(worker->sockfd), node->response->response->header);
                if (throwable->is_an_error(throwable)) {
                    get_log()->t(throwable);
                    pthread_exit(NULL); // TODO send an error message
                }
                header_write = TRUE;
            }

            // Gets the response lenght
            int response_lenght;
            ThrowablePtr throwable = str_to_int(node->response->response->req_content_len, &response_lenght);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
                pthread_exit(NULL); // TODO send an error message
            }

            get_log()->d(TAG_WORKER, "write_work response_lenght %d response_send_lenght %d", response_lenght, response_send_lenght);

            while (node->response->http_response_body == NULL) {
                get_log()->d(TAG_WORKER, "write_work unlock mutex");
                if (pthread_mutex_unlock(&(node->mutex)) != 0) {
                    return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work");
                }  
                usleep(100000);
                get_log()->d(TAG_WORKER, "write_work lock mutex");
                if (pthread_mutex_lock(&(node->mutex)) != 0) {
                    return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work");
                }
            }
            if (node->dimen != 0 && node->wrote == FALSE) {
                get_log()->d(TAG_WORKER, "write_work len %d != 0", node->dimen);
               
                throwable = send_request_total(&(worker->sockfd), node->response->http_response_body, node->dimen);
                if (throwable->is_an_error(throwable)) {
                    get_log()->t(throwable);
                    pthread_exit(NULL); // TODO send an error message
                }   
                get_log()->d(TAG_WORKER, "write_work scritto");

                response_send_lenght = response_send_lenght + node->dimen;

                node->wrote = TRUE;

                get_log()->d(TAG_WORKER, "write_work copiato");
            }

            if (response_lenght == response_send_lenght && response_send_lenght > 0) {
                get_log()->d(TAG_WORKER, "write_work dequeue");

                queue->dequeue(queue);
                response_send_lenght = 0;
                header_write = FALSE;
            }

            get_log()->d(TAG_WORKER, "write_work unlock mutex");
            if (pthread_mutex_unlock(&(node->mutex)) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "write_work");
            }  
        } 
    }
    return NULL;
}


void start_worker(int fd) {

    // initializing worker
    WorkerPtr worker = new_worker();

    worker->sockfd = fd;

    // Initializes queue
    RequestQueuePtr queue = init_request_queue();
    worker->requests_queue = queue;

    // initializing watchdog
    ThrowablePtr throwable = detach_watchdog(worker->watchdog);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    int watch_creation = pthread_create(&(worker->watch_thread), NULL, enable_watchdog, (void *) worker->watchdog);
    if (watch_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // initializing thread writer
    int w_creation = pthread_create(&(worker->writer_thread), NULL, write_work, (void *) worker);
    if (w_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // initializing thread reader
    int r_creation = pthread_create(&(worker->reader_thread), NULL, read_work, (void *) worker);
    if (r_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // detaching all the threads - no join, just condition waiting
    // (dev: the 'pthread_detach' does not change errno value, be aware!)
    if (pthread_detach(worker->watch_thread)   != 0 ||
        pthread_detach(worker->writer_thread)  != 0 ||
        pthread_detach(worker->reader_thread)  != 0  ) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(ESRCH), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // entering in mutex-condition loop
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
            break;
        }
    }
    mtx_lock_unlock = pthread_mutex_unlock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {

        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // checking for status of the threads and then exiting
    if (worker->watchdog->status      == STATUS_ERROR ||
        worker->writer_thread_status  == STATUS_ERROR ||
        worker->reader_thread_status  == STATUS_ERROR  ) {

        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}