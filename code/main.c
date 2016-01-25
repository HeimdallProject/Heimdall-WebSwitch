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

/*
 * ---------------------------------------------------------------------------
 * Function   : do_prefork
 * Description: Create worker child, the number is get from config file
 * ---------------------------------------------------------------------------
 */
 static ThrowablePtr do_prefork(){

    ConfigPtr config = get_config();
    
    int n_prefork = 0;
    ThrowablePtr throwable = str_to_int(config->pre_fork, &n_prefork);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
    }

    // if prefork is disabled we create anyway 1 child process
    if(n_prefork == 0){
        n_prefork = 1;
    }

    get_log()->i(TAG_MAIN, "Prefork %d worker", n_prefork);

    int children;
    for (children = 0; children < n_prefork; ++children){

        pid_t child_pid;
        errno = 0;

        child_pid = fork();
        if (child_pid == -1)
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "do_prefork"));

        // Child 
        if (child_pid == 0){

            // see worker.c
            start_worker();
            break;

        }else{

            get_log()->d(TAG_MAIN, "Created child n°%d - pid %ld", children, child_pid);

            HSharedMemPtr shm_mem = get_shm();
            ThrowablePtr throwable = shm_mem->add_worker_to_array(child_pid);
            if (throwable->is_an_error(throwable)) {
                get_log()->t(throwable);
            } 

            if (children == n_prefork - 1){
                ThrowablePtr throwable = shm_mem->print_worker_array();
                if (throwable->is_an_error(throwable)) {
                    get_log()->t(throwable);
                } 
            }
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "do_prefork()");
}

void cleaning(void){

    // Shared memory unlink
    if (shm_unlink(SHMOBJ_PATH) < 0)
        get_log()->i(TAG_HEIMDALL_SHM, "Error in shm_unlink");
    

    // Semaphore unlink
    if (sem_unlink(SHMOBJ_SEM) < 0)
        get_log()->i(TAG_HEIMDALL_SHM, "Error in sem_unlink");
}


/*
 * ---------------------------------------------------------------------------
 * MAIN PROGRAM
 * ---------------------------------------------------------------------------
 */
int main() {

    // Variable for errors
    ThrowablePtr throwable = NULL;

    // Initializes Config
    ConfigPtr config = get_config();
    if (config == NULL)
        exit(EXIT_FAILURE);

    // Initializes Log
    LogPtr log = get_log();
    if (log == NULL)
        exit(EXIT_FAILURE);

    // Initializes Scheduler
    SchedulerPtr scheduler = get_scheduler();
    if (scheduler == NULL)
        exit(EXIT_FAILURE);

    // Initializes Shared memory
    HSharedMemPtr shm_mem = get_shm();
    if (shm_mem == NULL)
        exit(EXIT_FAILURE);

    // Spawn child process
    throwable = do_prefork();
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    // Initializes Thread Pool
    ThreadPoolPtr th_pool = get_thread_pool();
    if (th_pool == NULL)
        exit(EXIT_FAILURE);

    // TODO pass limit from config
    set_fd_limit();

    int port = 0;
    throwable = str_to_int(config->server_main_port, &port);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
    }

    // Creates a new server
    int sockfd;
    throwable = create_server_socket(TCP, port, &sockfd);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    log->i(TAG_MAIN, "Created new server that is listening on port %d", port);

    // Starts listening for the clients
    int backlog = 0;
    throwable = str_to_int(config->backlog, &backlog);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
    }

    throwable = listen_to(sockfd, backlog);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    log->i(TAG_MAIN, "Ready to accept incoming connections...");

    // Register for cleaning at exit
    atexit(cleaning);

    int count = 0;

    // Starts to listen incoming connections
    while(TRUE) {

        // Accepts new connection
        int new_sockfd;
        throwable = accept_connection(sockfd, &new_sockfd);
        count++;
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        }

        while(TRUE){
            throwable = th_pool->add_fd_to_array(&new_sockfd);
            if (throwable->is_an_error(throwable)) {
                //get_log()->i(TAG_THREAD_POOL, "No space for FD available, wait for space.");
                throwable->destroy(throwable);
            }else{
                break;
            }
        }

        log->i(TAG_MAIN, "New connection accepted on socket number %d - total %d", new_sockfd, count);
    }
}