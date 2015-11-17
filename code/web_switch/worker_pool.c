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

#include "worker_pool.h"

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
 static int add_worker(WorkerPtr worker){
 	printf("%p\n", worker);
 	return 1;
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

 	wrkPoolPtr->delete_worker 	= delete_worker;
 	wrkPoolPtr->search_worker 	= search_worker;
 	wrkPoolPtr->add_worker 		= add_worker;

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








