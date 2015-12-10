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

#define TAG_WATCHDOG "WATCHDOG"
#define WATCH_OVER   42                             // so long and thanks for all the fish!
#define WATCH_TOWER  24                             // !hsif eht lla rof sknaht dna gnol os

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct watchdog_thread
 * Description      : this struct helps to manage and set attributes for the thread
 *                    which watch over the remote connection thread termination
 * ---------------------------------------------------------------------------
 */
typedef struct watchdog_thread {
    int status;                                     // watchdog operation status

    pthread_cond_t *worker_await_cond;              // worker condition to signal in case of watchover
    int *worker_await_flag;

    time_t killer_time;                             // time to schedule the watchdog wakeup
    time_t timeout_worker;                          // time to abort a thread run
    time_t timestamp_worker;                        // timestamp of the last worker operation
} Watchdog, *WatchdogPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : detach_watchdog
 * Description: This function detach the watchdog
 *
 * Param      : void pointer to the arg necessary - in this case the worker ptr
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr detach_watchdog(WatchdogPtr watchdog);

/*
 * ---------------------------------------------------------------------------
 * Function   : enable_watchdog
 * Description: This function runs the watchdog routine
 *
 * Param      : void pointer to the arg necessary - in this case the watchdog ptr
 * Return     :
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
