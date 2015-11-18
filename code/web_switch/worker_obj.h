//
//============================================================================
// Name             : worker_obj.h
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 16/11/2015
// Last modified    : 16/11/2015
// Description      : This header file contains infomation about worker object.
// ===========================================================================
//

#ifndef WORKER_OBJ_H
#define WORKER_OBJ_H

#include <stdio.h>
#include <stdlib.h>

// Macro for log message
#define TAG_WORKER_OBJ "WORKER_OBJ"

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct worker
 * Description  :
 *
 * Data:
 *
 * Functions:
 * ---------------------------------------------------------------------------
 */
typedef struct worker {
    char* thread_identifier;
    char* process_identifier;
} Worker, *WorkerPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : new_worker
 * Description: Method for worker creation.
 *
 * Param      :
 *
 * Return     :
 * ---------------------------------------------------------------------------
 */
Worker *new_worker();

#endif