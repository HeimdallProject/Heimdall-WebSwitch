#include "../include/thread_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable for handle main events.
 * ---------------------------------------------------------------------------
 */
static pthread_mutex_t mtx_wait_request 	= PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_wait_request 	= PTHREAD_COND_INITIALIZER;
static WorkerPoolPtr worker_pool_ptr 		= NULL;

static int worker_request = 0;

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Thread Pool
 * ---------------------------------------------------------------------------
 */
ThreadPoolPtr singleton_thdpool = NULL;

static void sigHandler(int sig){
    printf("Ouch! %d \n", sig);
}

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
 static void do_prefork(){

 	ConfigPtr config = get_config();
 	LogPtr log = get_log();

    int n_prefork;
    str_to_int(config->pre_fork, &n_prefork);
	log->d(TAG_THREAD_POOL, "Prefork %d worker", n_prefork);

	int worker_free = worker_pool_ptr->count_free_worker(worker_pool_ptr);
	log->d(TAG_THREAD_POOL, "There are n°%d free worker inside the pool", worker_free);

	int children = 0;
	int worker_to_create = n_prefork - worker_free;

	for (children = 0; children < worker_to_create; ++children){

		log->d(TAG_THREAD_POOL, "Create child n°%d", children);

		pid_t child_pid;

		child_pid = fork();
		if (child_pid == -1)
		    log->e(TAG_THREAD_POOL, "Error in do_prefork()");

		/* Child */
		if (child_pid == 0){
			signal(SIGCONT, sigHandler);
			pause();
		}

		WorkerPtr worker = new_worker(child_pid);
		worker_pool_ptr->add_worker(worker_pool_ptr, worker);
	}

	worker_pool_ptr->print_worker_pool(worker_pool_ptr);
 }

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

		do_prefork();
		
		/* Consume time! */
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

	// detach itself
	pthread_detach(pthread_self());

    // Init worker pool
    worker_pool_ptr = init_worker_pool();

    // Add pre_fork worker to the pool
    do_prefork();

    // enter in thread pool loop
    thread_pool_loop();

    // Never reached
    return arg;
}

/*
 *  See .h for more information.
 */
WorkerPtr get_worker() {

	int s = 0;
	WorkerPtr wrk_ptr = NULL;

	s = pthread_mutex_lock(&mtx_wait_request);
	if (s != 0)
		fprintf(stderr, "get_worker - Error in pthread_mutex_lock \n");
		
	// get worker from pool
	wrk_ptr = worker_pool_ptr->get_free_worker(worker_pool_ptr);

	/* Set condition request for perfomr something on the pool */
	worker_request = 1;

	s = pthread_mutex_unlock(&mtx_wait_request); 

	if (s != 0)
		fprintf(stderr, "get_worker - Error in pthread_mutex_unlock \n");
		
	/* Wake sleeping thread_pool */ 
	s = pthread_cond_signal(&cond_wait_request);
	if (s != 0)
		fprintf(stderr, "get_worker - Error in pthread_cond_signal \n");


	return wrk_ptr;
}

/*
 * ---------------------------------------------------------------------------
 * Function   	: init_thread_pool
 * Description	: Init thread pool singleton.
 *
 * Return     	: ThreadPoolPtr
 * ---------------------------------------------------------------------------
 */
ThreadPoolPtr init_thread_pool() {

	LogPtr log = get_log();
	log->d(TAG_THREAD_POOL, "Thread pool start.");

	pthread_t t1;
	int born;
	
	born = pthread_create(&t1, NULL, init_pool, NULL); 
	if (born != 0) {
		log->e(TAG_THREAD_POOL, "Error in pthread_create");
		return NULL;
	}

	ThreadPoolPtr th_pool = malloc(sizeof(ThreadPool));
    if (th_pool == NULL) {
    	log->e(TAG_THREAD_POOL, "Memory allocation error in init_thread_pool().");
        return NULL;
    }

    th_pool->thread_identifier = t1;
    th_pool->get_worker = get_worker;

    return th_pool;
}

/*
 *  See .h for more information.
 */
ThreadPoolPtr get_thread_pool() {

	if (singleton_thdpool == NULL) {
		singleton_thdpool = init_thread_pool();
    }

    // return singleton
    return singleton_thdpool;
}