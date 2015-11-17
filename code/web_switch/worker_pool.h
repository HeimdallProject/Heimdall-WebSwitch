//
//============================================================================
// Name             : worker_pool.h
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 17/11/2015
// Last modified    : 17/11/2015
// Description      : This header infomation about worker LL object.
// ===========================================================================
//

#ifndef WORKER_POOL_H
#define WORKER_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include "worker_obj.h"

#define TAG_WORKER_POOL_H "WORKER_POOL"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct worker_pool
 * Description      : This struct collect all functions pointers for managing
 					  a worker pool object.
 *
 * Functions:
 * ---------------------------------------------------------------------------
 */
typedef struct worker_pool {
    int (*add_worker)(WorkerPtr worker);
    int (*delete_worker)(WorkerPtr worker);
    int (*search_worker)(WorkerPtr worker);
    struct worker_pool* (*init_worker_pool)();
} WorkerPool, *WorkerPoolPtr;

/*
 * ---------------------------------------------------------------------------
 * Function     : init_worker_pool
 * Description  : Alloc and init struct worker_pool.
 *
 * Param        :
 *
 * Return       : 0 if ok, -1 on error.
 * ---------------------------------------------------------------------------
 */
 WorkerPoolPtr init_worker_pool();

#endif