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
#include "include/heimdall_shm.h"

#define TAG_MAIN "MAIN"

/*
 * ---------------------------------------------------------------------------
 * Function   : set_fd_limit
 * Description: Set max number of file descriptor that program can open.
 *
 * Param      : Number max of file descriptor
 *
 * Return     : void
 * ---------------------------------------------------------------------------
 */
static void set_fd_limit(){

    struct rlimit open_file_limit;

    // Query current soft/hard value 
    getrlimit(RLIMIT_NOFILE, &open_file_limit);

    // Set soft limit to hard limit
    open_file_limit.rlim_cur = open_file_limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &open_file_limit);
}

// /*
//  * ---------------------------------------------------------------------------
//  * Function   : worker_sig_handler
//  * Description: Function where worker start is execution when 
//                 receive signal SIGUSR1 from main.
//  * ---------------------------------------------------------------------------
//  */
// static void worker_sig_handler(int sig){
    
//     UNUSED(sig);
    
//     int *file_descriptor = malloc(sizeof(int));
//     if (file_descriptor == NULL) {
//         get_log()->e(TAG_THREAD_POOL, "Memory allocation error in worker_sig_handler!");
//         exit(EXIT_FAILURE);
//     }

//     // open unix socket for receice fd from thread pool
//     ThrowablePtr throwable = receive_fd(file_descriptor);
//     if (throwable->is_an_error(throwable)) {
//         get_log()->e(TAG_THREAD_POOL, "Error in receive_fd()");
//         get_log()->t(throwable);
//         exit(EXIT_SUCCESS);
//     }

//     get_log()->i(TAG_THREAD_POOL, "%ld riceived fd %d", (long)getpid(), *file_descriptor);

//     // see worker.c
//     start_worker(*file_descriptor);
//     return;
// }

/*
 * ---------------------------------------------------------------------------
 * Function   : do_prefork
 * Description: Create worker child, the number is get from config file
 * ---------------------------------------------------------------------------
 */
 static ThrowablePtr do_prefork(){

    ConfigPtr config = get_config();
    UNUSED(config);

    int n_prefork = 15;
    //str_to_int(config->pre_fork, &n_prefork); TOTO settato manualmente

    // TODO create at least one child if prefork is disabled

    get_log()->i(TAG_MAIN, "Prefork %d worker", n_prefork);

    int children;
    for (children = 0; children < n_prefork; ++children){

        /*get_log()->d(TAG_MAIN, "Create child n°%d", children);*/

        pid_t child_pid;
        errno = 0;

        child_pid = fork();
        if (child_pid == -1)
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "do_prefork"));

        // Child 
        if (child_pid == 0){
                        
            // ThrowablePtr throwable = set_signal(SIGUSR1, start_worker);
            // if (throwable->is_an_error(throwable)) {
            //     get_log()->t(throwable);
            //     // set signal failure, bye bye
            //     exit(EXIT_FAILURE);
            // }

            // pause();

            start_worker();
            break;

        }else{

            HSharedMemPtr shm_mem = get_shm();
            ThrowablePtr throwable = shm_mem->add_worker_to_array(child_pid);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
                exit(EXIT_SUCCESS);
            } 
            
/*            // last loop, print pool
            if(children == n_prefork - 1){
                shm_mem->print_worker_array();
            }*/

        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "do_prefork()");
 }

/*
 * ---------------------------------------------------------------------------
 * MAIN PROGRAM
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
    // ThreadPoolPtr th_pool = get_thread_pool();
    // if (th_pool == NULL)
    //     exit(EXIT_FAILURE);

    // Initializes Scheduler
    SchedulerPtr scheduler = get_scheduler();
    if (scheduler == NULL)
        exit(EXIT_FAILURE);

    // Initializes Shared memory
    HSharedMemPtr shm_mem = get_shm();
    if (shm_mem == NULL)
        exit(EXIT_FAILURE);

    log->i(TAG_MAIN, "Start main program");
    log->i(TAG_MAIN, "Config started");
    log->i(TAG_MAIN, "Log started");
    log->i(TAG_MAIN, "Thread Pool started");
    log->i(TAG_MAIN, "Scheduler started");

    // TODO pass limit from config
    set_fd_limit();

    // Spawn child process
    ThrowablePtr throwable = do_prefork();
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    // TODO maybe another value to set into config
    int port = 8080;  

    // Creates a new server
    int sockfd;
    throwable = create_server_socket(TCP, port, &sockfd);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    log->i(TAG_MAIN, "Created new server that is listening on port %d", port);

    // Starts listening for the clients
    throwable = listen_to(sockfd, 10);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    log->i(TAG_MAIN, "Ready to accept incoming connections...");

    // Starts to listen incoming connections
    while(TRUE) {

        log->i(TAG_MAIN, "Entering in loop");

        while(TRUE){

            int cc_conn = 0;

            throwable = shm_mem->get_concurrent_connection(&cc_conn);
            if (throwable->is_an_error(throwable)) {
                log->t(throwable);
            }

            // TODO get 15 from config
            if (cc_conn == 15) {
                log->i(TAG_MAIN, "No fd space available, wait for space.");
                usleep(500000);
            } else {
                break;
            }
        }

        // Accepts new connection
        int new_sockfd;
        throwable = accept_connection(sockfd, &new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        }

        throwable = shm_mem->add_fd_to_array(&new_sockfd);
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        };

        throwable = shm_mem->print_fd_array();
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
        } 

        pid_t worker_pid = 0;
        throwable = shm_mem->get_worker(&worker_pid);
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_SUCCESS);
        } 

        // wake up worker
        //kill(worker_pid, SIGUSR1);

        while (TRUE){
            throwable = send_fd(new_sockfd, worker_pid);
            if (throwable->is_an_error(throwable)) {
                get_log()->e(TAG_THREAD_POOL, "Failed attempt to send file descriptor to %ld", (long) worker_pid);
            } else {
                break;
            }
        }

        log->i(TAG_MAIN, "New connection accepted on socket number %d", new_sockfd);
    }
}