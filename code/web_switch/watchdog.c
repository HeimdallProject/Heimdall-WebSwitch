#include "../include/watchdog.h"

void *enable_watchdog(void *arg) {

    // retrieving watchdog
    Watchdog *watchdog = (Watchdog *) arg;

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
        watch_status = watch_over(watchdog, watchdog->requests->thread_id, watchdog->requests->timestamp, time(NULL));
        if (watch_status == STATUS_ERROR)
            return (void *) (intptr_t) STATUS_ERROR;
    }

    // successfull status
    return (void *) (intptr_t) STATUS_OK;
}

int watch_over(Watchdog *watchdog, pthread_t *running_thread, time_t running_timestamp, time_t current_timestamp) {

    // checking for timestamp distance and aborting thread if necessary
    time_t running_exec_time = current_timestamp - running_timestamp;
    if (running_exec_time > watchdog->timeout_worker) {
        int cancellation_status = pthread_cancel(*running_thread);
        if (cancellation_status != 0)
            return STATUS_ERROR;
    }

    // successfull status
    return STATUS_OK;
}
