//
//============================================================================
// Name             : worker_pool.c
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 17/11/2015
// Last modified    : 17/11/2015
// Description      : This file contains the implementation for Worker LL.
// ===========================================================================
//

#include "../include/worker_pool.h"

/*int length(NodePtr top) {

	int n = 0;
	NodePtr curr = top;

	while (curr != NULL) {
		n++;
		curr = curr -> next;
	}

	return n;
}

NodePtr search(NodePtr top, int key){

	while(top != NULL && key != top->num)
		top = top->next;

	return top;
}*/

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct worker_pool_node
 * Description      : Object used inside the worker pool.
 * ---------------------------------------------------------------------------
 */
typedef struct node {
	struct worker *worker;
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
static WorkerPoolNodePtr make_worker_pool_node(WorkerPtr worker){

	WorkerPoolNodePtr np = (WorkerPoolNodePtr) malloc(sizeof(WorkerPoolNode));
	np->worker = worker;
	np->next = NULL;

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
 		WorkerPoolNodePtr np = self->first;
		while (np != NULL) { 
			printf("Worker in lista: %p\n", np->worker);
			np = np->next;
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
 static int add_worker(WorkerPoolPtr self, WorkerPtr worker){
 	
 	WorkerPoolNodePtr nd = make_worker_pool_node(worker);

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
 * Function     : search_worker
 * Description  : Search worker inside the worker list.
 *
 * Param        :
 *   worker     : Worker pointer.
 *
 * Return       : 0 if ok, -1 on error.
 * ---------------------------------------------------------------------------
 */
 static int search_worker(WorkerPtr worker){
 	printf("%p\n", worker);
	return 1;
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
 static int delete_worker(WorkerPtr worker){
 	printf("%p\n", worker);
 	return 1;
 }

/*
 *  See .h for more information.
 */
 WorkerPoolPtr init_worker_pool(){

 	WorkerPoolPtr wrkPoolPtr = (WorkerPoolPtr) malloc(sizeof(WorkerPool));
	if (wrkPoolPtr == NULL) {
        fprintf(stderr, "Memory allocation error in init_worker_pool().\n");
        exit(EXIT_FAILURE);
    }

 	wrkPoolPtr->self                = wrkPoolPtr;
 	wrkPoolPtr->delete_worker 		= delete_worker;
 	wrkPoolPtr->search_worker 		= search_worker;
 	wrkPoolPtr->add_worker 			= add_worker;
 	wrkPoolPtr->print_worker_pool 	= print_worker_pool;
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








