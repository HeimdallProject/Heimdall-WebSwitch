#ifndef TYPES_H
#define TYPES_H

/*
 * this is only an example structure to define a circular buffer over a set of
 * finite web servers.
 */
typedef struct server {
    char *address;
    char *ip;
    int  port;
    int  weight;
    int  status;
} Server, *ServerPtr;

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct worker_pool
 * Description  : Struct contain structure for save the worker 
                  status in shared memory.
 * ---------------------------------------------------------------------------
 */
typedef struct thp_shared_mem {
    pid_t *worker_array; // Contains child process pid
    int *worker_busy; // Flag array for check wich process is ready (0) or busy (1)
    int *worker_counter; // Array where we keep track of the use of workers
    Server *worker_server; // Array where we keep track of the servers scheduled
} THPSharedMem, *THPSharedMemPtr;

#endif //TYPES_H