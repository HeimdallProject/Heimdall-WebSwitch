#include "../include/worker.h"

/*
 *  See .h for more information.
 */
WorkerPtr new_worker(pid_t os_id) {

    WorkerPtr wrk = malloc(sizeof(Worker));
    if (wrk == NULL) {
        fprintf(stderr, "Memory allocation error in new_log.\n");
        exit(EXIT_FAILURE);
    }

    // watchdog struct allocation
    wrk->watchdog = malloc(sizeof(Watchdog));
    if (wrk->watchdog == NULL) {
        fprintf(stderr, "Memory allocation error in new_log.\n");
        exit(EXIT_FAILURE);
    }

    wrk->worker_id = os_id;

    return wrk;
}

void *read_work(void *arg) {

    // casting the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    for (;;) {
        // TODO: reading and passing params to HTTPRequest setting the timestamp foreach request in the QUEUE
        worker->watchdog->timestamp_worker = time(NULL);
        // (DEV)
        for(;;);
    }

}

void *write_work(void *arg) {

    // casting the parameter
    WorkerPtr worker = (WorkerPtr) arg;
    return (void *) worker;
}


ThrowablePtr start_worker() {

    // initializing worker
    WorkerPtr worker = new_worker(0);

    // initialilizing the QUEUE data structure to manage a
    // node of the current request handled (pipeline-robust approach)
    // TODO: allocating using worker's own attribute


    // TODO: do something to allocate the QUEUE
    // initializing watchdog
    if (detach_watchdog(worker->watchdog) == STATUS_ERROR)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
    int watch_creation = pthread_create(&worker->watch_thread, NULL, enable_watchdog, (void *) worker->watchdog);
    if (watch_creation != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
    // (DEV)
    fprintf(stdout, "-> WATCHDOG DETACHED!\n");

    // initializing thread writer
    int w_creation = pthread_create(&worker->writer_thread, NULL, write_work, (void *) worker);
    if (w_creation != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
    // (DEV)
    fprintf(stdout, "-> WRITER CREATED!\n");

    // initializing thread reader
    int r_creation = pthread_create(&worker->reader_thread, NULL, read_work, (void *) worker);
    if (r_creation != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
    // (DEV)
    fprintf(stdout, "-> READER CREATED!\n");


    // waiting for the child threads running over their own routines
    int *watch_status;
    int *write_status;
    int *read_status;

    if (pthread_join(worker->watch_thread,  (void *) &watch_status)  == 0 ||
        pthread_join(worker->writer_thread, (void *) &write_status) == 0 ||
        pthread_join(worker->reader_thread, (void *) &read_status)  == 0  ) {

        if ((*watch_status) == STATUS_ERROR ||
            (*write_status) == STATUS_ERROR ||
            (*read_status)  == STATUS_ERROR  )
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
        else
            return get_throwable()->create(STATUS_OK, NULL, "start_worker");

    } else {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
    }
}