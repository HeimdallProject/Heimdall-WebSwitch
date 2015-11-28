#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "include/heimdall_config.h"
#include "include/thread_pool.h"
#include "include/log.h"
#include "include/connection.h"
#include "include/apache_status.h"
#include "include/throwable.h"

// Remove
#include <fcntl.h>

#define TAG_MAIN "MAIN"

/*
 * ---------------------------------------------------------------------------
 *
 * MAIN PROGRAM
 *
 * ---------------------------------------------------------------------------
 */
int main() {

    // Init Config
    ConfigPtr config = get_config();
    if(config == NULL)
        exit(EXIT_FAILURE);

    // Init Log
    LogPtr log = get_log();
    if(log == NULL)
        exit(EXIT_FAILURE);

    // Init Thread Pool
    ThreadPoolPtr th_pool = get_thread_pool();
    if(th_pool == NULL)
        exit(EXIT_FAILURE);

    // Init scheduler
    // TODO init scheduler

    log->d(TAG_MAIN, "Start main program");
    log->d(TAG_MAIN, "Config started at address %p", config);
    log->d(TAG_MAIN, "Log started at address %p", log);
    log->d(TAG_MAIN, "Thread Pool started at address %p", th_pool);
    //log->d(TAG_MAIN, "Scheduler started at address %p", scheduler);
/*
    int i = 0;
    for (i = 0; i < 15000; ++i){
        usleep(1);
    }

    // For test purpose only, fd must be the socket fd returned from accept
    int fd = open("/tmp/file.txt", O_RDONLY|O_CREAT|O_TRUNC, 0777);
    if (fd == -1){
        fprintf(stderr, "Error in open %s\n", strerror(errno));
    }

    ThrowablePtr throwable = th_pool->get_worker(fd);
    if (throwable->is_an_error(throwable)) {
        log->e(TAG_MAIN, "Error get_worker");
        exit(EXIT_SUCCESS);
    }*/

    // Creates a new server
    int port = 8080;  // TODO maybe another value to set into config
    // TODO port 80 doesn't work, maybe su privileges required
    int sockfd;
    ThrowablePtr throwable = create_server_socket(TCP, port, &sockfd);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    }

    log->d(TAG_MAIN, "Created new server that is listening on port %d", port);

    // Now start listening for the clients
    throwable = listen_to(sockfd, 5);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    }

    log->d(TAG_MAIN, "Ready to accept incoming connections...");

    // Start to listen incoming connections
    while(TRUE) {

        // Accept new connection
        int new_sockfd;
        throwable = accept_connection(sockfd, &new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        }

        // Pass socket to worker
        throwable= th_pool->get_worker(new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->e(TAG_MAIN, "Error get_worker");
            exit(EXIT_SUCCESS);
        }

        log->d(TAG_MAIN, "New connection accepted on socket number %d", new_sockfd);
    }
//
//    // initializing worker
//    WorkerPtr worker = new_worker();
//
//    // initialilizing the QUEUE data structure to manage a
//    // node of the current request handled (pipeline-robust approach)
//
//    // Initializes queue
//    RequestQueuePtr queue = init_request_queue();
//    worker->requests_queue = queue;
//
//    // Prepares buffer in order to receive the response
//    char *header = "GET / HTTP/1.1\r\nHost: 10.0.0.2:8080\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: it-IT,it;q=0.8,en-GB;q=0.6,en;q=0.4\r\n\r\n";
//    // Creates http_response structure
//    HTTPResponsePtr http_response = new_http_response();
//
//    // Creates http_request structure and put the request just read
//    HTTPRequestPtr http_request = new_http_request();
//    ThrowablePtr throwable = http_request->get_request(header, http_request, RQST);
//    if (throwable->is_an_error(throwable)) {
//        get_log()->t(throwable);
//        exit(EXIT_FAILURE);
//    }
//
//    http_response->response = http_request;
//
//    fprintf(stdout, "aooo2");
//    //TODO create thread
//
//    // Puts the http_response into a structure and puts the node into queue
//    RequestNodePtr node = init_request_node();
//    node->response = http_response;
//    queue->enqueue(queue, node);
//
//    fprintf(stdout, "aooo3");
//
//    return EXIT_SUCCESS;
}