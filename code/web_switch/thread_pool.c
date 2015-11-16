//
//============================================================================
// Name             : thread_pool.c
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 16/11/2015
// Last modified    : 16/11/2015
// Description      : This header file contains infomation about thread pool object.
// ===========================================================================
//

#include "thread_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Thread Pool
 * ---------------------------------------------------------------------------
 */
ThreadPool *singleton_thdpool = NULL;

/*
 * ---------------------------------------------------------------------------
 * Function   : init_pool
 * Description: 
 *
 * Param      :
 *   arg	  : Thread arg. Not used.
 *
 * Return     :
 * ---------------------------------------------------------------------------
 */

static void * init_pool(void *arg){

	char *s = (char *) arg;
    printf("%s", s);
    return (void *) strlen(s);
}

/*
 *  See .h for more information.
 */
ThreadPool *init_thread_pool() {

	Log log = *get_log();
	log.d(TAG_THREAD_POOL, "Thread pool start.");

	pthread_t t1;
	int born;
	
	born = pthread_create(&t1, NULL, init_pool, NULL); 
	if (born != 0) {
		log.e(TAG_THREAD_POOL, "Error in pthread_create");
		return NULL;
	}

	ThreadPool *th_pool = malloc(sizeof(ThreadPool));
    if (th_pool == NULL) {
    	log.e(TAG_THREAD_POOL, "Memory allocation error in thread_pool_init().");
        return NULL;
    }

    // Set pthread_t
    th_pool->thread_identifier = t1;

    return th_pool;
}

/*
 *  See .h for more information.
 */
ThreadPool *get_thread_pool() {

	if (singleton_thdpool == NULL) {
        // error
    }

    // return singleton
    return singleton_thdpool;
}