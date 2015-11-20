//
//============================================================================
// Name       : watchdog.h
// Description: This is the watchdog utils header file.
// ===========================================================================
//
#ifndef WEBSWITCH_WATCHDOG_H
#define WEBSWITCH_WATCHDOG_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "../include/helper.h"
#include "../include/request_queue.h"
#include "../include/worker.h"

#define TAG_WATCHDOG "WATCHDOG"
#define WATCH_OVER   42

/*
 * ---------------------------------------------------------------------------
 * Function   : detach_watchdog
 * Description: This function detach the watchdog
 *
 * Param      : void pointer to the arg necessary - in this case the worker ptr
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int detach_watchdog(WorkerPtr worker);

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct watchdog_thread
 * Description      : this struct helps to manage and set attributes for the thread
 *                    which watch over the remote connection thread termination
 * ---------------------------------------------------------------------------
 */
typedef struct watchdog_thread {
    pthread_t *thread_id;                           // thread identifier

    time_t killer_time;                             // time to schedule the watchdog wakeup
    time_t timeout_worker;                          // time to abort a thread run
    time_t timestamp_worker;                        // timestamp of the last worker operation
} Watchdog, *WatchdogPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : enable_watchdog
 * Description: This function runs the watchdog routine
 *
 * Param      : void pointer to the arg necessary - in this case the watchdog ptr
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
void *enable_watchdog(void *arg);

/*
 * ---------------------------------------------------------------------------
 * Function   : watch_over
 * Description: This function runs the watcher routine over the request threads
 *
 * Param      : watchdog ptr, current and long-running timestamp
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int watch_over(WatchdogPtr watchdog, time_t running_timestamp, time_t current_timestamp);

#endif //WEBSWITCH_WATCHDOG_H
