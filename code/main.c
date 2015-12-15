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

    // Initializes Config
    ConfigPtr config = get_config();
    if (config == NULL)
        exit(EXIT_FAILURE);

    // Initializes Log
    LogPtr log = get_log();
    if (log == NULL)
        exit(EXIT_FAILURE);

    // Initializes Thread Pool
    ThreadPoolPtr th_pool = get_thread_pool();
    if (th_pool == NULL)
        exit(EXIT_FAILURE);

    // Initializes scheduler
    // TODO Initializes scheduler

    log->i(TAG_MAIN, "Start main program");
    log->i(TAG_MAIN, "Config started");
    log->i(TAG_MAIN, "Log started");
    log->i(TAG_MAIN, "Thread Pool");
    //log->d(TAG_MAIN, "Scheduler started at address %p", scheduler);

    // Creates a new server
    int port = 8080;  // TODO maybe another value to set into config
    // TODO port 80 doesn't work, maybe su privileges required
    int sockfd;
    ThrowablePtr throwable = create_server_socket(TCP, port, &sockfd);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    }

    log->i(TAG_MAIN, "Created new server that is listening on port %d", port);

    // Starts listening for the clients
    throwable = listen_to(sockfd, 5);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    }

    log->i(TAG_MAIN, "Ready to accept incoming connections...");

    // Starts to listen incoming connections
    while(TRUE) {

        // Accepts new connection
        int new_sockfd;
        throwable = accept_connection(sockfd, &new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        }
        // TODO gestire gli stessi client!!
        // Passes socket to worker
        throwable = th_pool->get_worker(new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->e(TAG_MAIN, "Error get_worker");
            exit(EXIT_SUCCESS);
        }

        log->i(TAG_MAIN, "New connection accepted on socket number %d", new_sockfd);
    }
}