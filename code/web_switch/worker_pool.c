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
 * Function     : make_worker_pool_node
 * Description  : Make new node.
 *
 * Param        :
 *   worker     : Worker pointer.
 *
 * Return       : new node.
 * ---------------------------------------------------------------------------
 */
static WorkerPoolNodePtr make_worker_pool_node(pid_t worker_id){

	WorkerPoolNodePtr np 	= (WorkerPoolNodePtr) malloc(sizeof(WorkerPoolNode));
	np->worker_id 			= worker_id;
	np->next 				= NULL;
	np->free_for_job 		= TRUE;

	return np;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : add_worker
 * Description  : Add worker to list.
 *
 * Param        :
 *   worker     : Worker pointer.
 *
 * Return       : 0 if ok, -1 on error.
 * ---------------------------------------------------------------------------
 */
 static int print_worker_pool(WorkerPoolPtr self){

	if (self->first == NULL){
 		printf("List is empty \n");
 	}else{
 		WorkerPoolNodePtr nd = self->first;
		while (nd != NULL) { 
			printf("Worker in lista: %ld\n", (long)nd->worker_id);
			nd = nd->next;
		}
 	}

 	return 0;
 }

/*
 * ---------------------------------------------------------------------------
 * Function     : add_worker
 * Description  : Add worker to list.
 *
 * Param        :
 *   worker     : Worker pointer.
 *
 * Return       : 0 if ok, -1 on error.
 * ---------------------------------------------------------------------------
 */
 static int add_worker(WorkerPoolPtr self, pid_t worker_id){
 	
 	WorkerPoolNodePtr nd = make_worker_pool_node(worker_id);

 	if (self->first == NULL){
 		self->first = nd;
 		self->last = nd;
 	}else{
 		self->last->next = nd;
 		self->last = nd;
 	}
 	
 	return 0;
 }

/*
 * ---------------------------------------------------------------------------
 * Function     : count_free_worker
 * Description  : Return how many worker are waiting for a new job.
 *
 * Param        :
 *   worker     : Worker pool pointer.
 *
 * Return       : Number of free worker.
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

 	int count = 0;

 	WorkerPoolNodePtr nd = self->first;
	while(nd != NULL){

		if (nd->free_for_job == TRUE){
			nd->free_for_job = FALSE;
			break;
		}

		nd = nd->next;
		++count;
	}

	return nd->worker_id;
 }

/*
 * ---------------------------------------------------------------------------
 * Function     : delete_worker
 * Description  : Delete worker inside the worker list.
 *
 * Param        :
 *   worker     : Worker pointer.
 *
 * Return       : 0 if ok, -1 on error.
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

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
/*int main() {

	printf("Main\n");

	WorkerPoolPtr ptr = init_worker_pool();
	printf("%p\n", ptr);	

	ptr->add_worker(NULL);
	ptr->search_worker(NULL);
	ptr->delete_worker(NULL);

	return 0;

}*/








