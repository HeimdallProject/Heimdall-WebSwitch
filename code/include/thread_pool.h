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
#include <pthread.h>

#include "../include/log.h"
#include "../include/worker.h"
#include "../include/worker_pool.h"

#define TAG_THREAD_POOL "THREAD_POOL"

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
    Worker* (*get_worker)();
} ThreadPool, *ThreadPoolPtr;

/*
 * ---------------------------------------------------------------------------
 * Function     : init_thread_pool
 * Description  : Return thread identifier.
 *
 * Param        :
 *
 * Return       : Log typedef.
 * ---------------------------------------------------------------------------
 */
ThreadPoolPtr init_thread_pool();

#endif //THREAD_POOL_H