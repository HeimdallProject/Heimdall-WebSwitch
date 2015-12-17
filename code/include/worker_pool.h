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

#include "../include/macro.h"
#include "../include/log.h"

#define TAG_WORKER_POOL "WORKER_POOL"

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
 *
 * #include "../include/worker_pool.h"
 *
 int main() {

	printf("Main\n");

	WorkerPoolPtr ptr = init_worker_pool();
	printf("%p\n", ptr);	

	ptr->add_worker(NULL);
	ptr->delete_worker(NULL);

	return 0;

}*/

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

    ThrowablePtr (*add_worker)(struct worker_pool* self, pid_t worker_id);
    void (*delete_worker)(struct worker_pool* self, pid_t worker_id);
    pid_t (*get_free_worker)(struct worker_pool* self);
    int (*count_free_worker)(struct worker_pool* self);
    void (*print_worker_pool)();
} WorkerPool, *WorkerPoolPtr;

/*
 * ---------------------------------------------------------------------------
 * Function     : init_worker_pool
 * Description  : Alloc and init struct worker_pool.
 * ---------------------------------------------------------------------------
 */
 WorkerPoolPtr init_worker_pool();

#endif //WORKER_POOL_H