#include "../include/heimdall_shm.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Shared memory
 * ---------------------------------------------------------------------------
 */
HSharedMemPtr singleton_hshm = NULL;

//Do not set here but in config!
int number_of_worker = NULL; 

// sem used for access in shared memory
static sem_t * sem_id; 

/*
 * ---------------------------------------------------------------------------
 * Function   : print_fd_array
 * Description: Print fd array in shared memory
 *
 * Param      : 
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr print_worker_array(){

    if(sem_wait(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_wait", "add_fd_to_array");

    // Scan array and set fd to first position available
    int i;
    for (i = 0; i < number_of_worker; ++i){
        get_log()->i(TAG_HEIMDALL_SHM, "Worker %ld in position %d available %d", (long)worker_array[i], i, worker_busy[i]);
    }

    if(sem_post(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_post", "add_fd_to_array");

    return get_throwable()->create(STATUS_OK, NULL, "add_fd_to_array()");
}

/*
 * ---------------------------------------------------------------------------
 * Function   : add_fd_to_array
 * Description: Add file descriptor to fd_array in shared memory
 *
 * Param      : File descriptor
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr add_worker_to_array(pid_t worker_pid){

    get_log()->i(TAG_HEIMDALL_SHM, "Entro in add_worker_to_array");

    if(sem_wait(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_wait", "add_worker_to_array");

    // Scan array and set fd to first position available
    int i, flag = 0;
    for (i = 0; i < number_of_worker; ++i){
        if (worker_array[i] == 0){
            worker_array[i] = worker_pid;
            flag = 1;
            break;
        }
    }

    if(sem_post(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_post", "add_worker_to_array");

    if (flag == 0){
        return get_throwable()->create(STATUS_ERROR, "Cannot add worker_pid to array", "add_worker_to_array");        
    }else{
        return get_throwable()->create(STATUS_OK, NULL, "add_worker_to_array()");;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function   : get_worker
 * Description: Get the first worker ready.
 *
 * Param      : 
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr get_worker(pid_t *worker_pid){

    if(sem_wait(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_wait", "get_worker");

    // Scan array and get the first fd != 0
    int i, flag = 0;
    for (i = 0; i < number_of_worker; ++i){

        if (worker_busy[i] == 0){
            *worker_pid = worker_array[i];
            worker_busy[i] = 1;
            get_log()->i(TAG_HEIMDALL_SHM, "Get Worker %ld", (long)*worker_pid);
            flag = 1;
            break;
        }
    }

    if(sem_post(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_post", "get_worker");

    if (flag == 0){
        return get_throwable()->create(STATUS_ERROR, "Cannot get worker", "get_worker");        
    }else{
        return get_throwable()->create(STATUS_OK, NULL, "get_worker()");;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function   : end_job_worker
 * Description: Set worker with pid passed in parameter ready for new connection
 *
 * Param      : 
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr end_job_worker(pid_t worker_pid){

    if(sem_wait(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_wait", "end_job_worker");

    // Scan array and get the first fd != 0
    int i;
    for (i = 0; i < number_of_worker; ++i){

        if (worker_array[i] == worker_pid){
            worker_busy[i] = 0;
            get_log()->i(TAG_HEIMDALL_SHM, "Worker %ld ready for new conneciton", (long)worker_pid);
            break;
        }
    }

    if(sem_post(sem_id) == -1)
        return get_throwable()->create(STATUS_ERROR, "sem_post", "end_job_worker");

    return get_throwable()->create(STATUS_OK, NULL, "end_job_worker()");;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : init_shm
 * Description: Init POSIX shared memory and singleton
 * ---------------------------------------------------------------------------
 */
HSharedMemPtr init_shm(){

    ConfigPtr config = get_config();

    number_of_worker = 0;
    ThrowablePtr throwable = str_to_int(config->pre_fork, &number_of_worker);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        return NULL;
    }

    int size_worker_array   = sizeof(pid_t) * number_of_worker; 
    int size_worker_busy    = sizeof(int) * number_of_worker; 

    /* want shared segment capable of storing 1 struct heimdall_shm_data */
    int shared_seg_size = (size_worker_array + size_worker_busy);    
    
    // creating the shared memory object
    int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0){
        get_log()->e(TAG_HEIMDALL_SHM, "Error in shm_open - init_shm");
        return NULL;
    }

    get_log()->i(TAG_HEIMDALL_SHM, "Created shared memory object %s", SHMOBJ_PATH);

    // adjusting mapped file size
    if(ftruncate(shmfd, shared_seg_size) == -1){
        get_log()->e(TAG_HEIMDALL_SHM, "Error in ftruncate - init_shm");
        return NULL;
    }

    // Semaphore open
    sem_id = sem_open(SHMOBJ_SEM, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(sem_id == SEM_FAILED){
        get_log()->e(TAG_HEIMDALL_SHM, "Error in sem_open - init_shm");
        return NULL;
    }

    // requesting the shared segment mmap()
    void *start_mem = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (start_mem == NULL){
        get_log()->e(TAG_HEIMDALL_SHM, "Error in mmap - init_shm");
        return NULL;
    }

    worker_array   = (pid_t *)start_mem;    
    worker_busy    = (int *)start_mem + size_worker_array;

    get_log()->i(TAG_HEIMDALL_SHM, "Shared memory segment allocated correctly (%d bytes).", shared_seg_size);

    if(sem_wait(sem_id) == -1){
        get_log()->e(TAG_HEIMDALL_SHM, "Error in sem_wait - init_shm");
        return NULL;
    }

    // reset all array value to 0
    int i;
    for (i = 0; i < number_of_worker; ++i){
        worker_array[i] = 0;
        worker_busy[i]  = 0;
    }

    if(sem_post(sem_id) == -1){
        get_log()->e(TAG_HEIMDALL_SHM, "Error in sem_post - init_shm");
        return NULL;
    }

    // set singleton pointer
    singleton_hshm = malloc(sizeof(HSharedMem));
    if (singleton_hshm == NULL){
        get_log()->e(TAG_HEIMDALL_SHM, "Memory allocation error in init_shm!");
        return NULL;
    }

    singleton_hshm->add_worker_to_array = add_worker_to_array;
    singleton_hshm->print_worker_array  = print_worker_array;
    singleton_hshm->get_worker          = get_worker;
    singleton_hshm->end_job_worker      = end_job_worker;

    return singleton_hshm;
} 

/*
 *  See .h for more information.
 */
HSharedMemPtr get_shm() {

    if (singleton_hshm == NULL) {
        singleton_hshm = init_shm();
    }

    // return singleton
    return singleton_hshm;
}
