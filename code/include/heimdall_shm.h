//
//============================================================================
// Name        : heimdall_shm.h
// Description : This header file contains all the stuffs useful in order to
//               create and manage a POSIX shared memory for heimdall WebSwitch
// ===========================================================================
//

#ifndef HEIMDALL_SHM_H
#define HEIMDALL_SHM_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "../include/helper.h"
#include "../include/log.h"
#include "../include/throwable.h"

#define TAG_HEIMDALL_SHM "HEIMDALL_SHM_H"
#define SHMOBJ_PATH "/shmwebswitch"
#define SHMOBJ_SEM  "/heimdallsem"

// TODO prelevare dal config
#define N_WORKER 15

/*
 * ---------------------------------------------------------------------------
 * Variable in the shared segment
 * ---------------------------------------------------------------------------
 */

// Contains file descriptor that are to be processed
int *fd_array;

// Contains child process pid
pid_t *worker_array;

// Flag array for check wich process is ready (0) or busy (1)
int *worker_busy;

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct heimdall_shared_memory
 * Description  : Struct contain all method for manage the heimdall shared memory.
 * ---------------------------------------------------------------------------
 */
typedef struct heimdall_shared_memory {
    ThrowablePtr (*get_concurrent_connection)(int *concurrent_connection);
    
    ThrowablePtr (*add_fd_to_array)(int *fd);
    ThrowablePtr (*print_fd_array)();
    ThrowablePtr (*get_fd)(int *fd);

    ThrowablePtr (*add_worker_to_array)(pid_t worker_pid);
    ThrowablePtr (*print_worker_array)();
    ThrowablePtr (*get_worker)(pid_t *worker_pid);
    ThrowablePtr (*end_job_worker)(pid_t worker_pid);

} HSharedMem, *HSharedMemPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : init_shm
 * Description: Init POSIX shared memory
 * ---------------------------------------------------------------------------
 */
HSharedMemPtr get_shm();

#endif //HEIMDALL_SHM_H