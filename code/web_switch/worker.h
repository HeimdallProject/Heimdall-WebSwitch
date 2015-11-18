//
//============================================================================
// Name             : worker.h
// Author           : Alessio Moretti e Claudio Pastorini
// Version          : 0.2
// Data Created     : 11/11/2015
// Last modified    : 11/11/2015
// Description      : This is the webswitch single Worker thread body
// ===========================================================================
//
#ifndef WEBSWITCH_WORKER_H
#define WEBSWITCH_WORKER_H

#include <pthread.h>
#include <time.h>
#include "../utils/helper.h"
#include "watchdog.h"

// Macro for log message
#define TAG_WORKER "WORKER"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct thread_request
 * Description      : this struct manage the request
 * ---------------------------------------------------------------------------
 */
typedef struct thread_request {
    pthread_t *thread_id;                            // thread identifier
    time_t timestamp;                               // timestamp for watchdog check
} ThreadRequest;

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

    Watchdog *watchdog;
} Worker, *WorkerPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : start_worker
 * Description: This function runs the main loop into which the worker operates
 *              managing the connection between the client and the remote machine
 *
 * Param      : WorkerPtr
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int start_worker(WorkerPtr worker);

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

#endif //WEBSWITCH_WORKER_H
