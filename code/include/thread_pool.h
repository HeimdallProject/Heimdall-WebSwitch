//
//============================================================================
// Name             : thread_pool.h
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 16/11/2015
// Last modified    : 16/11/2015
// Description      : This header file contains infomation about thread pool object.
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
	struct thread_pool *self;
    pthread_t thread_identifier;
    Worker* (*get_worker)();
} ThreadPool;

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
ThreadPool *init_thread_pool();

#endif //THREAD_POOL_H