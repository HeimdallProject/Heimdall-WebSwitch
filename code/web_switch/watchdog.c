#include "../include/watchdog.h"
#include <syscall.h>

ThrowablePtr detach_watchdog(WatchdogPtr watchdog) {
    
    // setting up watchdog
    // retrieving the config params for the watchdog and converting them
    Config *config = get_config();
    
    long k_time;
    long out_time;

    ThrowablePtr throwable = str_to_long(config->killer_time, &k_time);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "detach_watchdog");
    }

    throwable = str_to_long(config->timeout_worker, &out_time);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "detach_watchdog");
    }

    // watchdog wake-up time
    watchdog->killer_time = (time_t) k_time;
    // setting up the execution time
    watchdog->timeout_worker = (time_t) out_time;
    return get_throwable()->create(STATUS_OK, NULL, "detach_watchdog");;
}

void *enable_watchdog(void *arg) {

    int sid = syscall(SYS_gettid);
    get_log()->d(TAG_WATCHDOG, "watch_creation %d", sid);

    // retrieving watchdog
    WatchdogPtr watchdog = (WatchdogPtr) arg;

    // initializing time specifics
    struct timespec *req_time = malloc(sizeof(struct timespec));
    struct timespec *rem_time = malloc(sizeof(struct timespec));
    if (req_time == NULL || rem_time == NULL) {
        watchdog->status = STATUS_ERROR;
        pthread_cond_signal(watchdog->worker_await_cond);
        return NULL;
    }

    // starting watch-over-thread loop main routine
    int sleep_status;
    int watch_status;
    for (;;) {
        // sleeping loop
        // setting req_time specifics
        req_time->tv_sec  = watchdog->killer_time / 1000000000;
        req_time->tv_nsec = (long) watchdog->killer_time;
        while (TRUE) {
            sleep_status = nanosleep(req_time, rem_time);
            // upon not successfull complete nanosleep
            if (sleep_status != 0) {
                if (errno == EFAULT) {
                    watchdog->status = STATUS_ERROR;
                    pthread_cond_signal(watchdog->worker_await_cond);
                    free(req_time);
                    free(rem_time);
                    return NULL;
                } else {
                    req_time->tv_nsec = rem_time->tv_nsec;
                }
            }
            // upon successfull complete nanosleep -> watchover routine
            if (sleep_status == 0)
                break;
        }

        // watchover routine
        watch_status = watch_over(watchdog, watchdog->timestamp_worker, time(NULL));
        if (watch_status == WATCH_OVER) {
            watchdog->status = STATUS_OK;
            pthread_cond_signal(watchdog->worker_await_cond);
            free(req_time);
            free(rem_time);
            return NULL;
        }
    }
}

int watch_over(WatchdogPtr watchdog, time_t running_timestamp, time_t current_timestamp) {
    // checking for timestamp distance and aborting thread if necessary
    time_t running_exec_time = current_timestamp - running_timestamp;
    // we wait at least timeout_seconds + 1 before exiting the thread
    if (running_exec_time > watchdog->timeout_worker) {
        *watchdog->worker_await_flag = WATCH_OVER;
        get_log()->d(TAG_WATCHDOG, "\nWATCH OVER\n");
        return WATCH_OVER;
    }
    else
        return STATUS_OK;
}

WatchdogPtr new_watchdog() {
    WatchdogPtr watchdog = malloc(sizeof(Watchdog));
    if (watchdog == NULL) {
        get_log()->e(TAG_WATCHDOG, "Memory allocation error in new_watchdog!");
        exit(EXIT_FAILURE);
    }

    return watchdog;
}
