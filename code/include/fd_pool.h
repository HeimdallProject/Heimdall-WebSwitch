//
//============================================================================
// Name       : fd_pool.h
// Description: This header infomation about file descriptor LL object.
// ===========================================================================
//
#ifndef FD_POOL_H
#define FD_POOL_H

#include <stdio.h>
#include <stdlib.h>

#include "../include/macro.h"
#include "../include/log.h"

#define TAG_FD_POOL "FD_POOL"

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
 *
 * #include "../include/worker_pool.h"
 *
 int main() {

	printf("Main\n");

	FdPoolPtr ptr = init_fd_pool();
	printf("%p\n", ptr);	

	ptr->add_fd(0);
	ptr->delete_fd(0);

	return 0;

}*/

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct fd_pool
 * Description      : This struct collect all functions pointers for managing
 					  a file descriptor pool object.
 *
 * Functions:
 * ---------------------------------------------------------------------------
 */
typedef struct fd_pool{
	struct node *first;
    struct node *last;

    ThrowablePtr (*add_fd)(struct fd_pool* self, int file_descriptor);
    int (*get_front_fd)(struct fd_pool* self);
    void (*delete_fd)(struct fd_pool* self, int file_descriptor);
    void (*print_fd_pool)();
} FdPool, *FdPoolPtr;

/*
 * ---------------------------------------------------------------------------
 * Function     : init_fd_pool
 * Description  : Alloc and init struct fd_pool.
 * ---------------------------------------------------------------------------
 */
 FdPoolPtr init_fd_pool();

#endif //FD_POOL_H