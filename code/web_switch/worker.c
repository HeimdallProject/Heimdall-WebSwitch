//
//============================================================================
// Name             : worker.c
// Author           : Alessio Moretti e Claudio Pastorini
// Version          : 0.2
// Data Created     : 11/11/2015
// Last modified    : 11/11/2015
// Description      : This is the webswitch single Worker thread body
// ===========================================================================
//

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "worker.h"


// initializing the watchdog struct
Watchdog *watchdog;

// initializing the requests buffer (DEV)
ThreadRequest *requests;

int start_worker() {

    // initializing write thread
    // TODO: do something to start the thread

    // setting up watchdog
    // struct allocation
    watchdog = malloc(sizeof(Watchdog));
    if (watchdog == NULL)
        return STATUS_ERROR;
    // struct init
    watchdog->requests = requests;
    // thread initialization
    int watchdog_creation = pthread_create(watchdog->thread_id, NULL, enable_watchdog, NULL);
    if (watchdog_creation != 0)
        return STATUS_ERROR;

    // initialilizing the FIFO data structure to manage a
    // node of the current request handled (pipeline-robust approach)
    // TODO: do something to allocate the FIFO

    // starting the main routine cycling in a read/execute loop
    for (;;) {
        // TODO: reading and passing params to HTTPRequest setting the timestamp foreach request in the FIFO


    }

    // successfull status
    return STATUS_OK;
}


void *enable_watchdog(void *arg) {

    // initializing time specifics
    struct timespec *req_time = malloc(sizeof(struct timespec));
    struct timespec *rem_time = malloc(sizeof(struct timespec));
    if (req_time == NULL || rem_time == NULL)
        return (void *) (intptr_t) STATUS_ERROR;


    // starting watch-over-thread loop main routine
    int sleep_status;
    int watch_status;
    for (;;) {
        // TODO: advancing step
        // sleeping loop
        // setting req_time specifics
        req_time->tv_nsec = watchdog->killer_time;
        while (TRUE) {
            sleep_status = nanosleep(req_time, rem_time);
            // upon not successfull complete nanosleep
            if (sleep_status == -1) {
                if (errno == EFAULT)
                    return (void *) (intptr_t) STATUS_ERROR;
                if (errno == EINTR)
                    req_time->tv_nsec = rem_time->tv_nsec;
            }
            // upon successfull complete nanosleep -> watchover routine
            if (sleep_status == 0)
                break;
        }

        // watchover routine
        watch_status = watch_over(watchdog->requests->thread_id, watchdog->requests->timestamp, time(NULL));
        if (watch_status == STATUS_ERROR)
            return (void *) (intptr_t) STATUS_ERROR;
    }

    // successfull status
    return (void *) (intptr_t) STATUS_OK;
}

int watch_over(pthread_t *running_thread, time_t running_timestamp, time_t current_timestamp) {

    // checking for timestamp distance and aborting thread if necessary
    time_t running_exec_time = current_timestamp - running_timestamp;
    if (running_exec_time > watchdog->exec_time) {
        int cancellation_status = pthread_cancel(*running_thread);
        if (cancellation_status != 0)
            return STATUS_ERROR;
    }

    // successfull status
    return STATUS_OK;
}

