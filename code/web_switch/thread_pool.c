#include "../include/thread_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable
 * * ---------------------------------------------------------------------------
 */

// Sinlgeton Thread Pool
static ThreadPoolPtr singleton_thdpool = NULL;

//Do not set here but in config!
static int max_fd = NULL; 

// FD Array
static int *fd_array;

// Mutex for access to fd_array
static pthread_mutex_t mtx_wait_request = PTHREAD_MUTEX_INITIALIZER;

/*
 * ---------------------------------------------------------------------------
 * Function   : add_fd_to_array
 * Description: Add file descriptor to fd_array
 *
 * Param      : File descriptor
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr add_fd_to_array(int *fd){

	int s = 0;
	s = pthread_mutex_lock(&mtx_wait_request);
	if (s != 0)
		get_log()->e(TAG_THREAD_POOL, "Error in pthread_mutex_lock");

    int i, flag = 0;
    for (i = 0; i < max_fd; ++i){
        if (fd_array[i] == 0){
            fd_array[i] = *fd;
            flag = 1;
            break;
        }
    }

	s = pthread_mutex_unlock(&mtx_wait_request); 
	if (s != 0)
		get_log()->e(TAG_THREAD_POOL, "Error in pthread_mutex_unlock");

	if (flag == 0){
        return get_throwable()->create(STATUS_ERROR, "Cannot add fd to array", "add_fd_to_array");        
    }else{
        return get_throwable()->create(STATUS_OK, NULL, "add_fd_to_array()");;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function   : print_fd_array
 * Description: Print fd array
 *
 * Param      : 
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr print_fd_array(){

    int i;
    for (i = 0; i < max_fd; ++i){
        get_log()->i(TAG_HEIMDALL_SHM, "FD %d in position %d", fd_array[i], i);
    }

    return get_throwable()->create(STATUS_OK, NULL, "print_fd_array");
}


/*
 * ---------------------------------------------------------------------------
 * Function   : get_fd
 * Description: Passing to pointer in parameter a new fd socket
 *
 * Param      : 
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr get_fd(int *fd_ptr){

	int s = 0;
	s = pthread_mutex_lock(&mtx_wait_request);
	if (s != 0)
		get_log()->e(TAG_THREAD_POOL, "Error in pthread_mutex_lock");

    // Scan array and get the first fd != 0
    int i, flag = 0;
    for (i = 0; i < max_fd; ++i){

        if (fd_array[i] != 0){
            *fd_ptr = fd_array[i];
            fd_array[i] = 0;
            flag = 1;
            break;
        }
    }

	s = pthread_mutex_unlock(&mtx_wait_request); 
	if (s != 0)
		get_log()->e(TAG_THREAD_POOL, "Error in pthread_mutex_unlock");

    if (flag == 0){
        return get_throwable()->create(STATUS_ERROR, "Cannot get fd", "get_fd");        
    }else{
        return get_throwable()->create(STATUS_OK, NULL, "get_fd()");;
    }
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

	HSharedMemPtr shm_mem = get_shm();

	for (;;) {

		/*ThrowablePtr  throwable = shm_mem->print_worker_array();
		if (throwable->is_an_error(throwable)) {
        	get_log()->t(throwable);
		} */

		int fd = 0;
        ThrowablePtr throwable = get_fd(&fd);
        if (throwable->is_an_error(throwable)) {
            //get_log()->i(TAG_THREAD_POOL, "No fd to serve.");
            throwable->destroy(throwable);
            continue;
        }

        pid_t worker_pid = 0;

        while(TRUE){

			// get worker from shared memory
	        throwable = shm_mem->get_worker(&worker_pid);
	        if (throwable->is_an_error(throwable)) {
	            //get_log()->i(TAG_THREAD_POOL, "No Worker available, wait for space.");
	            throwable->destroy(throwable);
	            continue;
	        } else{
	        	break;
	        }
        }

		int attempt = 0;

		while (attempt <= MAX_SEND_ATTEMPT){
		            
			throwable = send_fd(fd, worker_pid);
			if (throwable->is_an_error(throwable)) {
				//get_log()->e(TAG_THREAD_POOL, "Failed attempt %d to send file descriptor to %ld", attempt, (long) worker_pid);
				throwable->destroy(throwable);
				attempt++;
			}else{
				break;
			}
		}

		if (attempt == 5){
			kill(worker_pid, SIGKILL);
			get_log()->e(TAG_THREAD_POOL, "Failed all attempts to send file descriptor to %ld", (long) worker_pid);
		}

		// close fd from main side
		throwable = close_connection(fd);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
        }
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

	ConfigPtr config = get_config();

    max_fd = 0;
    ThrowablePtr throwable = str_to_int(config->max_fd, &max_fd);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    // Init static array and save pointer to global variable
    int array[max_fd];
    fd_array = array;

	int i;
    for (i = 0; i < max_fd; ++i){
		fd_array[i] = 0;
    }

    // enter in thread pool loop
    thread_pool_loop();

    // Never reached
    return arg;
}


/*
 * ---------------------------------------------------------------------------
 * Function   	: init_thread_pool
 * Description	: Init thread pool singleton.
 *
 * Return     	: ThreadPoolPtr
 * ---------------------------------------------------------------------------
 */
static ThreadPoolPtr init_thread_pool() {

	get_log()->i(TAG_THREAD_POOL, "Thread pool start.");

	pthread_t t1;
	int born;
	
	born = pthread_create(&t1, NULL, init_pool, NULL); 
	if (born != 0) {
		get_log()->e(TAG_THREAD_POOL, "Error in pthread_create");
		return NULL;
	}

	ThreadPoolPtr th_pool = malloc(sizeof(ThreadPool));
    if (th_pool == NULL) {
    	get_log()->e(TAG_THREAD_POOL, "Memory allocation error in init_thread_pool!");
        return NULL;
    }

    th_pool->thread_identifier 	= t1;
    th_pool->add_fd_to_array 	= add_fd_to_array;
    th_pool->print_fd_array 	= print_fd_array;

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