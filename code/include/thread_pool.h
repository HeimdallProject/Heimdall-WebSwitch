//
//============================================================================
// Name       : thread_pool.h
// Description: This header file contains information about thread pool object.
// ===========================================================================
//
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "../include/heimdall_shm.h"
#include "../include/worker.h"
#include "../include/log.h"
#include "../include/heimdall_config.h"
#include "../include/message_controller.h"
#include "../include/throwable.h"
#include "../include/helper.h"
#include "../include/connection.h"

#define TAG_THREAD_POOL "THREAD_POOL"
#define MAX_SEND_ATTEMPT 5

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct thread_pool
 * Description  : 
 *
 * Data:
 *
 * Functions:
 * ---------------------------------------------------------------------------
 */
typedef struct thread_pool {
    pthread_t thread_identifier;
    ThrowablePtr (*add_fd_to_array)(int *fd);
    ThrowablePtr (*print_fd_array)();
} ThreadPool, *ThreadPoolPtr;

/*
 * ---------------------------------------------------------------------------
 * Function     : get_thread_pool
 * Description  : Return thread pool singleton pointer.
 *
 * Return       : ThreadPoolPtr
 * ---------------------------------------------------------------------------
 */
ThreadPoolPtr get_thread_pool();

#endif //THREAD_POOL_H