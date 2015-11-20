
#include "../include/thread_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable for handle main events.
 * ---------------------------------------------------------------------------
 */
static pthread_mutex_t mtx_wait_request = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_wait_request = PTHREAD_COND_INITIALIZER;
static WorkerPoolPtr worker_pool_ptr = NULL;

static int worker_request = 0;

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Thread Pool
 * ---------------------------------------------------------------------------
 */
ThreadPool *singleton_thdpool = NULL;

/*
 * ---------------------------------------------------------------------------
 * Function   : thread_pool_loop
 * Description: 
 *
 * Param      :
 *
 * Return     :
 * ---------------------------------------------------------------------------
 */
static void thread_pool_loop(){

	for (;;) {
		
		int s = 0;

		s = pthread_mutex_lock(&mtx_wait_request);
		if (s != 0)
			fprintf(stderr, "thread_pool_loop - Error in pthread_mutex_lock \n");
		
		while (worker_request == 0) { 
			/* Wait for something */ 
			s = pthread_cond_wait(&cond_wait_request, &mtx_wait_request);
			if (s != 0)
				fprintf(stderr, "thread_pool_loop - Error in pthread_cond_wait \n");
		}
		
		/* Consume time! */
		printf("BRUCIATE!");

		worker_request = 0;
		
		s = pthread_mutex_unlock(&mtx_wait_request); 
		if (s != 0)
			fprintf(stderr, "thread_pool_loop - Error in pthread_mutex_unlock \n");
		
		/* Perhaps do other work here that doesn't require mutex lock */ 
	}
}

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
static void *init_pool(void *arg){

	char *s = (char *) arg;
    printf("%s", s);

    // Init worker pool
    worker_pool_ptr = init_worker_pool();
    worker_pool_ptr->print_worker_pool(worker_pool_ptr);

    thread_pool_loop();

    // pthread_detach(pthread_self());

    // Never reached
    return (void *) strlen(s);
}

/*
 *  See .h for more information.
 */
Worker *get_worker() {

	int s = 0;

	s = pthread_mutex_lock(&mtx_wait_request);
	if (s != 0)
		fprintf(stderr, "get_worker - Error in pthread_mutex_lock \n");
		
	/* Condition request */
	worker_request = 1;

	s = pthread_mutex_unlock(&mtx_wait_request); 

	if (s != 0)
		fprintf(stderr, "get_worker - Error in pthread_mutex_unlock \n");
		
	/* Wake sleeping thread_pool */ 
	s = pthread_cond_signal(&cond_wait_request);
	if (s != 0)
		fprintf(stderr, "get_worker - Error in pthread_cond_signal \n");

	Worker *wrk = new_worker();
	return wrk;
}

/*
 *  See .h for more information.
 */
ThreadPool *init_thread_pool() {

	LogPtr log = get_log();
	log->d(TAG_THREAD_POOL, "Thread pool start.");

	pthread_t t1;
	int born;
	
	born = pthread_create(&t1, NULL, init_pool, "HELL!!!!!\n"); 
	if (born != 0) {
		log->e(TAG_THREAD_POOL, "Error in pthread_create");
		return NULL;
	}

	ThreadPool *th_pool = malloc(sizeof(ThreadPool));
    if (th_pool == NULL) {
    	log->e(TAG_THREAD_POOL, "Memory allocation error in thread_pool_init().");
        return NULL;
    }

    th_pool->thread_identifier = t1;
    th_pool->get_worker = get_worker;
    th_pool->self = th_pool;

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