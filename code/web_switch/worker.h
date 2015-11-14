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
 * Structure        : typedef struct watchdog_thread
 * Description      : this struct helps to manage and set attributes for the thread
 *                    which watch over the remote connection thread termination
 * ---------------------------------------------------------------------------
 */
typedef struct watchdog_thread {
    pthread_t *thread_id;                           // thread identifier
    long killer_time;                               // time to schedule the watchdog wakeup
    time_t exec_time;                               // time to abort a thread run

    struct thread_request *requests;                // pointer to the request array (DEV)
} Watchdog;



/*
 * ---------------------------------------------------------------------------
 * Function   : start_worker
 * Description: This function runs the main loop into which the worker operates
 *              managing the connection between the client and the remote machine
 *
 * Param      :
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int start_worker();

/*
 * ---------------------------------------------------------------------------
 * Function   : enable_watchdog
 * Description: This function runs the watchdog routine
 *
 * Param      :
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
void *enable_watchdog(void *arg);

/*
 * ---------------------------------------------------------------------------
 * Function   : watch_over
 * Description: This function runs the watcher routine over the request threads
 *
 * Param      :
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int watch_over(pthread_t *running_thread, time_t running_timestamp, time_t current_timestamp);

#endif //WEBSWITCH_WORKER_H
