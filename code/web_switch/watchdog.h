//
// Created by Odysseus on 18/11/15.
//

#ifndef WEBSWITCH_WATCHDOG_H
#define WEBSWITCH_WATCHDOG_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "../utils/helper.h"
#include "worker.h"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct watchdog_thread
 * Description      : this struct helps to manage and set attributes for the thread
 *                    which watch over the remote connection thread termination
 * ---------------------------------------------------------------------------
 */
typedef struct watchdog_thread {
    pthread_t *thread_id;                           // thread identifier
    time_t killer_time;                               // time to schedule the watchdog wakeup
    time_t timeout_worker;                          // time to abort a thread run

    struct thread_request *requests;                // pointer to the request array (DEV)
} Watchdog;


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
 * Param      : watchdog ptr, ptr to running thread_id, current and long-running timestamp
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int watch_over(Watchdog *watchdog, pthread_t *running_thread, time_t running_timestamp, time_t current_timestamp);

#endif //WEBSWITCH_WATCHDOG_H
