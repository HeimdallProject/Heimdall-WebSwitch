#include "../include/worker_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct worker_pool_node
 * Description      : Object used inside the worker pool.
 * ---------------------------------------------------------------------------
 */
typedef struct node {
	pid_t worker_id;
	int free_for_job;

	struct node *next;
} WorkerPoolNode, *WorkerPoolNodePtr;

/*
 * ---------------------------------------------------------------------------
 * Function     : add_worker
 * Description  : Add worker node to list.
 *
 * Param        :
 *   pid_t     	: Worker process id
 *
 * Return       : A Throwable.
 * ---------------------------------------------------------------------------
 */
 static ThrowablePtr add_worker(WorkerPoolPtr self, pid_t worker_id){

 	WorkerPoolNodePtr nd 	= (WorkerPoolNodePtr) malloc(sizeof(WorkerPoolNode));
 	if (nd == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error", "add_worker");
    }

	nd->worker_id 			= worker_id;
	nd->next 				= NULL;
	nd->free_for_job 		= TRUE;
 	
 	if (self->first == NULL){
 		self->first = nd;
 		self->last = nd;
 	}else{
 		self->last->next = nd;
 		self->last = nd;
 	}
 	
    return get_throwable()->create(STATUS_OK, NULL, "add_worker");
 }

/*
 * ---------------------------------------------------------------------------
 * Function     : count_free_worker
 * Description  : Return how many worker are waiting for a new job.
 *
 * Param        :
 *
 * Return       : int - number of free worker.
 * ---------------------------------------------------------------------------
 */
 static int count_free_worker(WorkerPoolPtr self){

 	int count = 0;

 	WorkerPoolNodePtr nd = self->first;
	while(nd != NULL){
		
		if (nd->free_for_job == TRUE)
			++count;

		nd = nd->next;
	}

	return count;
 }

 /*
 * ---------------------------------------------------------------------------
 * Function     : get_free_worker
 * Description  : Return one worker ready for the job.
 *
 * Param        :
 *   worker     : Worker pool pointer.
 *
 * Return       : Retunr
 * ---------------------------------------------------------------------------
 */
 static pid_t get_free_worker(WorkerPoolPtr self){

 	WorkerPoolNodePtr nd = self->first;

	while(nd != NULL){

		if (nd->free_for_job == TRUE){
			nd->free_for_job = FALSE;
			break;
		}

		nd = nd->next;
	}

	return nd->worker_id;
 }

/*
 * ---------------------------------------------------------------------------
 * Function     : delete_worker
 * Description  : Delete worker inside the worker list.
 *
 * Param        :
 *   pid_t     	: process identifier id to delete
 *
 * Return       : void
 * ---------------------------------------------------------------------------
 */
 static void delete_worker(WorkerPoolPtr self, pid_t worker_id){

 	WorkerPoolNodePtr nd 	= self->first;
	WorkerPoolNodePtr prev 	= NULL;

	while(nd != NULL){

		if (nd->worker_id == worker_id){

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
 * Function     : print_worker_pool
 * Description  : Print worker pool
 *
 * Param        :
 *
 * Return       : void
 * ---------------------------------------------------------------------------
 */
 static void print_worker_pool(WorkerPoolPtr self){

	if (self->first == NULL){
 		get_log()->e(TAG_WORKER_POOL, "List is empty");
 	}else{
 		WorkerPoolNodePtr nd = self->first;
		while (nd != NULL) { 
			get_log()->e(TAG_WORKER_POOL, "Worker: %lu", (long)nd->worker_id);
			nd = nd->next;
		}
 	}
 }

/*
 *  See .h for more information.
 */
 WorkerPoolPtr init_worker_pool(){

 	WorkerPoolPtr wrkPoolPtr = (WorkerPoolPtr) malloc(sizeof(WorkerPool));
	if (wrkPoolPtr == NULL) {
        fprintf(stderr, "Memory allocation error in init_worker_pool!");
        exit(EXIT_FAILURE);
    }

 	wrkPoolPtr->delete_worker 		= delete_worker;
 	wrkPoolPtr->add_worker 			= add_worker;
 	wrkPoolPtr->print_worker_pool 	= print_worker_pool;
 	wrkPoolPtr->get_free_worker 	= get_free_worker;
 	wrkPoolPtr->count_free_worker 	= count_free_worker;
 	wrkPoolPtr->first				= NULL;
 	wrkPoolPtr->last                = NULL;

 	return wrkPoolPtr;
 }








