#include "../include/fd_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct node
 * Description      : Object used inside the fd pool.
 * ---------------------------------------------------------------------------
 */
typedef struct node {
	int file_descriptor;

	struct node *next;
} FdPoolNode, *FdPoolNodePtr;

/*
 * ---------------------------------------------------------------------------
 * Function     		: add_fd
 * Description  		: Add fd node to list.
 *
 * Param        		:
 *   file_descriptor	: file descriptor to save inside node.
 *
 * Return     			: A Throwable.
 * ---------------------------------------------------------------------------
 */
 static ThrowablePtr add_fd(FdPoolPtr self, int file_descriptor){

 	FdPoolNodePtr nd = (FdPoolNodePtr) malloc(sizeof(FdPoolNode));
	if (nd == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error", "add_fd");
    }

	nd->file_descriptor = file_descriptor;
	nd->next 			= NULL;

 	if (self->first == NULL){
 		self->first = nd;
 		self->last = nd;
 	}else{
 		self->last->next = nd;
 		self->last = nd;
 	}
 	
    return get_throwable()->create(STATUS_OK, NULL, "add_fd");
 }

 /*
 * ---------------------------------------------------------------------------
 * Function     : get_front_fd
 * Description  : Return a file descriptor inside the first node.
 *
 * Param        :
 *
 * Return       : int
 * ---------------------------------------------------------------------------
 */
 static int get_front_fd(FdPoolPtr self){

 	FdPoolNodePtr nd = self->first;
	return nd->file_descriptor;
 }

/*
 * ---------------------------------------------------------------------------
 * Function     	: delete_fd
 * Description  	: Delete node with file descriptor specified in second parameter.
 *
 * Param        	:
 *	file_descriptor	: int
 *
 * Return       	: void
 * ---------------------------------------------------------------------------
 */
 static void delete_fd(FdPoolPtr self, int file_descriptor){

 	FdPoolNodePtr nd 	= self->first;
	FdPoolNodePtr prev 	= NULL;

	while(nd != NULL){

		if (nd->file_descriptor == file_descriptor){

			// if first
			if (prev == NULL){
				self->first = nd->next;
				free(nd);
				break;
			}

			prev->next = nd->next;
			free(nd);
			break;
		}

		prev 	= nd;
		nd 		= nd->next;
	}
 }

 /*
 * ---------------------------------------------------------------------------
 * Function     : print_fd_pool
 * Description  : Print fd pool.
 *
 * Param        :
 *
 * Return       : void
 * ---------------------------------------------------------------------------
 */
 static void print_fd_pool(FdPoolPtr self){

	if (self->first == NULL){
 		get_log()->e(TAG_FD_POOL, "List is empty");
 	}else{
 		FdPoolNodePtr nd = self->first;
		while (nd != NULL) { 
			get_log()->e(TAG_FD_POOL, "FD: %lu", (long)nd->file_descriptor);
			nd = nd->next;
		}
 	}
 }

/*
 *  See .h for more information.
 */
 FdPoolPtr init_fd_pool(){

 	FdPoolPtr fd_pool = (FdPoolPtr) malloc(sizeof(FdPool));
    if (fd_pool == NULL) {
        get_log()->e(TAG_FD_POOL, "Memory allocation error in init_fd_pool!");
        exit(EXIT_FAILURE);
    }

 	fd_pool->add_fd 		= add_fd;
 	fd_pool->get_front_fd 	= get_front_fd;
 	fd_pool->delete_fd 		= delete_fd;
 	fd_pool->print_fd_pool 	= print_fd_pool;
 	fd_pool->first			= NULL;
 	fd_pool->last           = NULL;

 	return fd_pool;
 }








