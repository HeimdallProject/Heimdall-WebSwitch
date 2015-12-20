#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

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

    // Initializes scheduler - first call to singleton
    SchedulerPtr scheduler = get_scheduler();


    log->i(TAG_MAIN, "Start main program");
    log->i(TAG_MAIN, "Config started");
    log->i(TAG_MAIN, "Log started");
    log->i(TAG_MAIN, "Thread Pool started");
    log->d(TAG_MAIN, "Scheduler started");

    struct rlimit open_file_limit;

    // Query current soft/hard value 
    getrlimit(RLIMIT_NOFILE, &open_file_limit);

    // Set soft limit to hard limit
    open_file_limit.rlim_cur = open_file_limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &open_file_limit);

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

        // Passes socket to worker
        throwable = th_pool->get_worker(new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->e(TAG_MAIN, "Error get_worker");
            exit(EXIT_SUCCESS);
        } 

        log->i(TAG_MAIN, "New connection accepted on socket number %d", new_sockfd);
    }
}