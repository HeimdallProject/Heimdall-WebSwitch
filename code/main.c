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
#include "include/shared_mem.h"
#include "include/types.h"
#include "include/macro.h"

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
 * Function   : cleaning
 * Description: Perform some cleaning action at exit
 *
 * Return     : void
 * ---------------------------------------------------------------------------
 */
void cleaning(int signum){

    // Shared memory unlink
    if (shm_unlink(WRK_SHM_PATH) < 0)
        get_log()->i(TAG_MAIN, "Error in shm_unlink");

    // Semaphore unlink
    if (sem_unlink(WRK_SEM_PATH) < 0)
        get_log()->i(TAG_MAIN, "Error in sem_unlink");

    // Terminate program
    exit(signum);
}

/*
 * ---------------------------------------------------------------------------
 * Function   : do_prefork
 * Description: Create worker child and shared memory for keep track of child PID.
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

    int total_size = 0;
    total_size+=sizeof(THPSharedMem);
    total_size+=sizeof(pid_t)*n_prefork; // Array worker
    total_size+=sizeof(int)*n_prefork; // Array busy
    total_size+=sizeof(int)*n_prefork; // Array counter
    total_size+=sizeof(Server)*n_prefork; // Array server

    // Initializes Shared memory
    void *start_mem = init_shm(WRK_SHM_PATH, total_size, WRK_SEM_PATH);
    if (start_mem == NULL)
        exit(EXIT_FAILURE);

    get_log()->i(TAG_THREAD_POOL, "Shared memory start from %p", start_mem);

    // Mappgin shared memory segment
    THPSharedMemPtr worker_pool = start_mem;
    worker_pool->worker_array   = start_mem+sizeof(THPSharedMem);
    worker_pool->worker_busy    = start_mem+sizeof(THPSharedMem)+sizeof(pid_t)*n_prefork;
    worker_pool->worker_counter = start_mem+sizeof(THPSharedMem)+sizeof(pid_t)*n_prefork+sizeof(int)*n_prefork;
    worker_pool->worker_server  = start_mem+sizeof(THPSharedMem)+sizeof(pid_t)*n_prefork+sizeof(int)*n_prefork+sizeof(int)*n_prefork;

    int i = 0;
    for (i = 0; i < n_prefork; ++i){
        worker_pool->worker_array[i]    = 0;
        worker_pool->worker_busy[i]     = 0;
        worker_pool->worker_counter[i]  = 0;
    }

    // Get sem for access in shared memory
    sem_t *sem = sem_open(WRK_SEM_PATH, 0);

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

            if(sem_wait(sem) == -1)
                return get_throwable()->create(STATUS_ERROR, "sem_wait", "do_prefork");

            // Scan array and set fd to first position available
            int i, flag = 0;
            for (i = 0; i < n_prefork; ++i){
                if (worker_pool->worker_array[i] == 0){
                    worker_pool->worker_array[i] = child_pid;
                    flag = 1;
                    break;
                }
            }

            if(sem_post(sem) == -1)
                return get_throwable()->create(STATUS_ERROR, "sem_post", "do_prefork");

            if (flag == 0){
                return get_throwable()->create(STATUS_ERROR, "Cannot add worker_pid to array", "do_prefork");        
            }            
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

    signal(SIGINT, cleaning);

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