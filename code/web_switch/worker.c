//
//============================================================================
// Name             : worker.c
// Author           : Alessio Moretti, Claudio Pastorini e Andrea Cerra
// Version          : 0.2
// Data Created     : 11/11/2015
// Last modified    : 18/11/2015
// Description      : This is the webswitch single Worker thread body
// ===========================================================================
//

#include "../include/worker.h"

/*
 *  See .h for more information.
 */
WorkerPtr new_worker() {

    Worker *wrk = malloc(sizeof(Worker));
    if (wrk == NULL) {
        fprintf(stderr, "Memory allocation error in new_log.\n");
        exit(EXIT_FAILURE);
    }

    // Set attributes
    // TODO: choose attributes

    return wrk;
}

void *read_work(void *arg) {

    // casting the parameter
    WorkerPtr worker = (WorkerPtr) arg;

    for (;;) {
        // TODO: reading and passing params to HTTPRequest setting the timestamp foreach request in the QUEUE
        // (DEV)
        worker->watchdog->timestamp_worker = time(NULL);
    }

}

void *write_work(void *arg) {

    // casting the parameter
    WorkerPtr worker = (WorkerPtr) arg;

}


ThrowablePtr start_worker() {

    // initializing worker
    WorkerPtr worker = new_worker();

    // initialilizing the QUEUE data structure to manage a
    // node of the current request handled (pipeline-robust approach)
    // TODO: allocating using worker's own attribute


    // TODO: do something to allocate the QUEUE
    // initializing watchdog
    if (detach_watchdog(worker) == STATUS_ERROR)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");

    // initializing thread writer
    int w_creation = pthread_create(worker->writer, NULL, write_work, (void *) worker);
    if (w_creation != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");

    // initializing thread reader
    int r_creation = pthread_create(worker->reader, NULL, write_work, (void *) worker);
    if (r_creation != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");

    // waiting for the child threads running over their own routines
    void *watch_status;
    void *write_status;
    void *read_status;

    if (pthread_join(*(worker->watchdog->thread_id), &watch_status) == 0 ||
        pthread_join(*(worker->writer), &write_status)              == 0 ||
        pthread_join(*(worker->reader), &read_status)               == 0  ) {

        if (((intptr_t) watch_status) == STATUS_ERROR ||
            ((intptr_t) write_status) == STATUS_ERROR ||
            ((intptr_t) read_status)  == STATUS_ERROR  )
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
        else
            return get_throwable()->create(STATUS_OK, NULL, "start_worker");

    } else {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker");
    }
}