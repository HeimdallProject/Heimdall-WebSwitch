#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "include/heimdall_config.h"
#include "include/thread_pool.h"
#include "include/log.h"
#include "include/connection.h"
#include "include/apache_status.h"
#include "include/throwable.h"

#define TAG_MAIN    "MAIN"

/*
 * ---------------------------------------------------------------------------
 * Message structure for messages in the shared segment
 * ---------------------------------------------------------------------------
 */

/*struct shared_data {
    int conc_connections;
};*/

/*
 * ---------------------------------------------------------------------------
 * Global variable for shared memory
 * ---------------------------------------------------------------------------
 */
// static int shmfd;                                               /* shared memory file descriptor */
// static int shared_seg_size = (1 * sizeof(struct shared_data));  /* want shared segment capable of storing 1 message */
// static struct shared_data *shared_msg;                          /* the shared segment, and head of the messages list */
// static sem_t * sem_id;                                          /* sem used for access in shared memory */

// void signal_callback_handler(int signum)
// {

//         /**
//          * Semaphore unlink: Remove a named semaphore  from the system.
//          */
//         if ( shm_unlink(SHMOBJ_PATH) < 0 )
//         {
//                 perror("shm_unlink");
//         }

//         /**
//          * Semaphore Close: Close a named semaphore
//          */
//         if ( sem_close(sem_id) < 0 )
//         {
//             perror("sem_close");
//         }

//         /**
//          * Semaphore unlink: Remove a named semaphore  from the system.
//          */
//         if ( sem_unlink(SHMOBJ_SEM) < 0 )
//         {
//             perror("sem_unlink");
//         }
//    // Terminate program
//    exit(signum);
// }


// ThrowablePtr set_shm(){
    
//     // creating the shared memory object
//     shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
//     if (shmfd < 0){
//         return get_throwable()->create(STATUS_ERROR, "shm_open", "set_shm");
//     }

//     get_log()->d(TAG_MAIN, "Created shared memory object %s", SHMOBJ_PATH);

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

//     get_log()->d(TAG_MAIN, "Shared memory segment allocated correctly (%d bytes).", shared_seg_size);

//     if(sem_wait(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_wait", "set_shm");
//     }

//     get_log()->d(TAG_MAIN, "Set shared memory to 0");

//     shared_msg->conc_connections = 0;

//     if(sem_post(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_post", "set_shm");
//     }

//     get_log()->d(TAG_MAIN, "Sem closed");

//     return get_throwable()->create(STATUS_OK, NULL, "set_shm()");;
// } 

// ThrowablePtr update_shm(){

//     if(sem_wait(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_wait", "update_shm");
//     }

//     get_log()->d(TAG_MAIN, "Open sem for update shared memory");

//     shared_msg->conc_connections = shared_msg->conc_connections+1;

//     get_log()->d(TAG_MAIN, "The total of concurrent connections are %d",shared_msg->conc_connections);

//     if(sem_post(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_post", "update_shm");
//     }

//     get_log()->d(TAG_MAIN, "Sem closed");

//     return get_throwable()->create(STATUS_OK, NULL, "update_shm()");;
// }

// ThrowablePtr get_shm(int *total_concurrent_connection){

//     *total_concurrent_connection = -1;

//     if(sem_wait(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_wait", "get_shm");
//     }

//     get_log()->d(TAG_MAIN, "Open sem for get shared memory");

//     *total_concurrent_connection = shared_msg->conc_connections;

//     get_log()->d(TAG_MAIN, "From get the total of concurrent connections are %d",shared_msg->conc_connections);

//     if(sem_post(sem_id) == -1){
//         return get_throwable()->create(STATUS_ERROR, "sem_post", "get_shm");
//     }

//     get_log()->d(TAG_MAIN, "Sem closed in get");

//     return get_throwable()->create(STATUS_OK, NULL, "get_shm()");;
// }

void set_fd_limit(){

    struct rlimit open_file_limit;

    // Query current soft/hard value 
    getrlimit(RLIMIT_NOFILE, &open_file_limit);

    // Set soft limit to hard limit
    open_file_limit.rlim_cur = open_file_limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &open_file_limit);
}

/*
 * ---------------------------------------------------------------------------
 * MAIN PROGRAM
 * ---------------------------------------------------------------------------
 */
int main() {

    //signal(SIGINT, signal_callback_handler);

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

    // Initializes Scheduler
    SchedulerPtr scheduler = get_scheduler();
    if (scheduler == NULL)
        exit(EXIT_FAILURE);

    log->i(TAG_MAIN, "Start main program");
    log->i(TAG_MAIN, "Config started");
    log->i(TAG_MAIN, "Log started");
    log->i(TAG_MAIN, "Thread Pool started");
    log->i(TAG_MAIN, "Scheduler started");

    // Create shared memory when we keep track of the total for concurrent connection
    /*ThrowablePtr throwable = set_shm();
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } */

    // TODO pass limit from config
    set_fd_limit();

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
    throwable = listen_to(sockfd, 10);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    } 

    log->i(TAG_MAIN, "Ready to accept incoming connections...");

    int *concurrent_conn = malloc(sizeof(int));
    if (concurrent_conn == NULL) {
        log->e(TAG_MAIN, "Memory allocation error in main!");
        exit(EXIT_FAILURE);
    }

    // Starts to listen incoming connections
    while(TRUE) {

        log->i(TAG_MAIN, "Entering in loop");

        /*throwable = get_shm(concurrent_conn);
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        } */

        //usleep(1000000);

        /*if(*concurrent_conn == 1000){
            log->i(TAG_MAIN, "Too many connection %d, waiting for space", *concurrent_conn);
            usleep(1000000);
            continue;
        }*/

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

        /*throwable = update_shm();
        if (throwable->is_an_error(throwable)) {
            log->t(throwable);
            exit(EXIT_FAILURE);
        }*/
    }
}