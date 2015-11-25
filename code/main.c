#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "include/heimdall_config.h"
#include "include/thread_pool.h"
#include "include/log.h"
#include "include/connection.h"
#include "include/apache_status.h"

#define TAG_MAIN "MAIN"

/*
 * ---------------------------------------------------------------------------
 *
 * MAIN PROGRAM
 *
 * ---------------------------------------------------------------------------
 */
int main() {

    /* Init Config */
    ConfigPtr config = get_config();
    if(config == NULL)
        exit(EXIT_FAILURE);

    /* Init Log */
    LogPtr log = get_log();
    if(log == NULL)
        exit(EXIT_FAILURE);

    /* Init Thread Pool */
    ThreadPoolPtr th_pool = get_thread_pool();
    if(th_pool == NULL)
        exit(EXIT_FAILURE);

    /* Init scheduler */
    // TODO init scheduler

    log->d(TAG_MAIN, "Start main program");
    log->d(TAG_MAIN, "Config started at address %p", config);
    log->d(TAG_MAIN, "Log started at address %p", log);
    log->d(TAG_MAIN, "Thread Pool started at address %p", th_pool);
    //log->d(TAG_MAIN, "Scheduler started at address %p", scheduler);

/*    int i = 0;
    for (i = 0; i < 15000; ++i){
        usleep(1);
    }

    WorkerPtr wrk = th_pool->get_worker();
    printf("Worker: %p\n", wrk);
    for (i = 0; i < 10000; ++i){
        usleep(1);
    }

    printf("Send signal \n");
    kill(wrk->worker_id, SIGCONT);

    for (i = 0; i < 10000; ++i){
        usleep(1);
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

        // Pass socket to worker // TODO
        WorkerPtr wrk = th_pool->get_worker();

        log->d(TAG_MAIN, "New connection accepted on socket number %d passed to worker: %p", new_sockfd, wrk);
    }
}