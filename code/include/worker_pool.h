//
//============================================================================
// Name       : worker_pool.h
// Description: This header infomation about worker LL object.
// ===========================================================================
//
#ifndef WORKER_POOL_H
#define WORKER_POOL_H

#include <stdio.h>
#include <stdlib.h>

#include "../include/worker.h"

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
typedef struct worker_pool{
	struct node *first;
    struct node *last;
    struct worker_pool* self;
    int (*add_worker)(struct worker_pool* self, WorkerPtr worker);
    int (*delete_worker)(WorkerPtr worker);
    int (*search_worker)(WorkerPtr worker);
    int (*print_worker_pool)();
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

#endif //WORKER_POOL_H