//
//============================================================================
// Name             : watchdog.c
// Author           : Alessio Moretti
// Version          : 0.2
// Data Created     : 18/11/2015
// Last modified    : 18/11/2015
// Description      : This is the watchdog utils source file
// ===========================================================================
//

#include "../include/watchdog.h"

int detach_watchdog(WorkerPtr worker) {
    // setting up watchdog
    // struct allocation
    worker->watchdog = malloc(sizeof(Watchdog));
    if (worker->watchdog == NULL)
        return STATUS_ERROR;
    // retrieving the config params for the watchdog and converting them
    Config *config = get_config();
    long k_time;
    long out_time;
    if (str_to_long(config->killer_time, &k_time)->is_an_error(get_throwable())      ||
        str_to_long(config->timeout_worker, &out_time)->is_an_error(get_throwable()))
        return STATUS_ERROR;
    // watchdog wake-up time
    worker->watchdog->killer_time = (time_t) k_time;
    // setting up the execution time
    worker->watchdog->timeout_worker = (time_t) out_time;
    // thread initialization
    int watchdog_creation = pthread_create(worker->watchdog->thread_id, NULL, enable_watchdog, (void *) worker->watchdog);
    if (watchdog_creation != 0)
        return STATUS_ERROR;
    else
        return STATUS_OK;
}

void *enable_watchdog(void *arg) {

    // retrieving watchdog
    WatchdogPtr watchdog = (WatchdogPtr) arg;

    // initializing time specifics
    struct timespec *req_time = malloc(sizeof(struct timespec));
    struct timespec *rem_time = malloc(sizeof(struct timespec));
    if (req_time == NULL || rem_time == NULL)
        return (void *) (intptr_t) STATUS_ERROR;


    // starting watch-over-thread loop main routine
    int sleep_status;
    int watch_status;
    for (;;) {
        // sleeping loop
        // setting req_time specifics
        req_time->tv_nsec = (long) watchdog->killer_time;
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
        watch_status = watch_over(watchdog, watchdog->timestamp_worker, time(NULL));
        if (watch_status == STATUS_ERROR)
            return (void *) (intptr_t) STATUS_ERROR;
        if (watch_status == WATCH_OVER)
            return (void *) (intptr_t) STATUS_OK;
    }
}

int watch_over(WatchdogPtr watchdog, time_t running_timestamp, time_t current_timestamp) {

    // checking for timestamp distance and aborting thread if necessary
    time_t running_exec_time = current_timestamp - running_timestamp;
    if (running_exec_time > watchdog->timeout_worker)
        return WATCH_OVER;
    else
        return STATUS_OK;
}
