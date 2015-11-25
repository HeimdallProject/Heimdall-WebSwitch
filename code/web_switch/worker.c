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

    // initializing condition and mutex
    if (pthread_cond_init(&wrk->await_cond, NULL) != 0)
        exit(EXIT_FAILURE);
    if (pthread_mutex_init(&wrk->await_mtx, NULL) != 0)
        exit(EXIT_FAILURE);

    // presetting threads attributes
    wrk->worker_await_flag = WATCH_TOWER;

    wrk->reader_thread_status = STATUS_OK;
    wrk->writer_thread_status = STATUS_OK;
    wrk->watchdog->status     = STATUS_OK;

    wrk->watchdog->worker_await_cond = &wrk->await_cond;
    wrk->watchdog->worker_await_flag = &wrk->worker_await_flag;

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
    worker = worker;
    for(;;);
}


void start_worker() {

    // initializing worker
    WorkerPtr worker = new_worker(0);

    // initialilizing the QUEUE data structure to manage a
    // node of the current request handled (pipeline-robust approach)
    // TODO: do something to allocate the QUEUE

    // initializing watchdog
    ThrowablePtr  throwable = detach_watchdog(worker->watchdog)
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    int watch_creation = pthread_create(&worker->watch_thread, NULL, enable_watchdog, (void *) worker->watchdog);
    if (watch_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // initializing thread writer
    int w_creation = pthread_create(&worker->writer_thread, NULL, write_work, (void *) worker);
    if (w_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // initializing thread reader
    int r_creation = pthread_create(&worker->reader_thread, NULL, read_work, (void *) worker);
    if (r_creation != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // detaching all the threads - no join, just condition waiting
    // (dev: the 'pthread_detach' does not change errno value, be aware!)
    if (pthread_detach(worker->watch_thread)   != 0 ||
        pthread_detach(worker->writer_thread)  != 0 ||
        pthread_detach(worker->reader_thread)  != 0  ) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(ESRCH), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // entering in mutex-condition loop
    int awaiting;
    int mtx_lock_unlock = pthread_mutex_lock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    while (worker->worker_await_flag == WATCH_TOWER) {
        awaiting = pthread_cond_wait(&worker->await_cond, &worker->await_mtx);
        if (awaiting != 0) {
            get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
            exit(EXIT_FAILURE);
        }
        else
            break;
    }
    mtx_lock_unlock = pthread_mutex_unlock(&worker->await_mtx);
    if (mtx_lock_unlock != 0) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }

    // checking for status of the threads and then exiting
    if (worker->watchdog->status      == STATUS_ERROR ||
        worker->writer_thread_status  == STATUS_ERROR ||
        worker->reader_thread_status  == STATUS_ERROR  ) {
        get_log()->t(get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "start_worker"));
        exit(EXIT_FAILURE);
    }
    else
        exit(EXIT_SUCCESS);
}