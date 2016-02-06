#include "../include/shared_mem.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : init_shm
 * Description: Init POSIX shared memory segment
 * ---------------------------------------------------------------------------
 */
void *init_shm(char* shared_name, size_t size, char* sem_name){
    
    sem_t *sem = NULL;

    // creating the shared memory object
    int shmfd = shm_open(shared_name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0){
        get_log()->e(TAG_SHARED_MEM, "Error in shm_open - init_shm");
        return NULL;
    }

    get_log()->i(TAG_SHARED_MEM, "Created shared memory object %s", shared_name);

    // adjusting mapped file size
    if(ftruncate(shmfd, size) == -1){
        get_log()->e(TAG_SHARED_MEM, "Error in ftruncate - init_shm");
        return NULL;
    }

    // Semaphore open
    sem = sem_open(sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(sem == SEM_FAILED){
        get_log()->e(TAG_SHARED_MEM, "Error in sem_open - init_shm");
        return NULL;
    }

    // requesting the shared segment mmap()
    void *struct_shm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (struct_shm == NULL){
        get_log()->e(TAG_SHARED_MEM, "Error in mmap - init_shm");
        return NULL;
    }

    get_log()->i(TAG_SHARED_MEM, "Shared memory segment %s allocated correctly (%d bytes) at address %p.", 
                                    shared_name, 
                                    size, 
                                    struct_shm);

    return struct_shm;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : get_shm
 * Description: Get POSIX shared memory segment
 * ---------------------------------------------------------------------------
 */
void *get_shm(char* shared_name){

    struct stat sb;

    // creating the shared memory object
    int shmfd = shm_open(shared_name, O_RDWR, 0);
    if (shmfd < 0){
        get_log()->e(TAG_SHARED_MEM, "Error in shm_open - get_shm");
        return NULL;
    }

    if (fstat(shmfd, &sb) == -1){
        get_log()->e(TAG_SHARED_MEM, "Error in fstat - get_shm");
        return NULL;
    }
        
    void *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, shmfd, 0); 
    if (addr == MAP_FAILED){
        get_log()->e(TAG_SHARED_MEM, "Error in mmap - get_shm");
        return NULL;
    }

    get_log()->i(TAG_SHARED_MEM, "Get shared memory object %s at %p size %d", shared_name, addr, sb.st_size);

    return addr;
}