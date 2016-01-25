//
//============================================================================
// Name        : shared_mem.h
// Description : This header file contains all the stuffs useful in order to
//               create and manage a POSIX shared memory for heimdall WebSwitch
// ===========================================================================
//

#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "../include/log.h"

#define TAG_SHARED_MEM "SHARED_MEM"

/*
 * ---------------------------------------------------------------------------
 * Function   : get_shm
 * Description: Init POSIX shared memory segment
 * ---------------------------------------------------------------------------
 */
void *init_shm(char* shared_name, size_t size, char* sem_name);
void *get_shm(char* shared_name);

#endif //SHARED_MEM_H