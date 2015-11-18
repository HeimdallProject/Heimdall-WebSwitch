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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "../config/config.h"
#include "../utils/helper.h"
#include "config_parser.h"
#include "worker.h"

#include "watchdog.h"
#include "worker_obj.h"

// initializing the requests buffer (DEV)
ThreadRequest *requests;

/*
 *  See .h for more information.
 */
WorkerPtr new_worker() {

    Worker *wrk = malloc(sizeof(Worker));
    if (wrk == NULL) {
        fprintf(stderr, "Memory allocation error in new_log.\n");
        exit(EXIT_FAILURE);
    }

    // Set attribute
    wrk->thread_identifier = NULL;
    wrk->process_identifier = NULL;

    return wrk;
}

int start_worker(WorkerPtr worker) {

    // initializing write thread
    // TODO: do something to start the thread

    // setting up watchdog
    // struct allocation
    worker->watchdog = malloc(sizeof(Watchdog));
    if (worker->watchdog == NULL)
        return STATUS_ERROR;
    // struct init
    worker->watchdog->requests = requests;
    // retrieving the config params for the watchdog and converting them
    Config *config = get_config();
    long k_time;
    long out_time;
    if (str_to_long(config->killer_time, &k_time)->is_an_error(get_throwable())      ||
        str_to_long(config->timeout_worker, &out_time)->is_an_error(get_throwable())  )
        return STATUS_ERROR;
    // watchdog wake-up time
    worker->watchdog->killer_time      = (time_t) k_time;
    // setting up the execution time
    worker->watchdog->timeout_worker   = (time_t) out_time;
    // thread initialization
    int watchdog_creation = pthread_create(worker->watchdog->thread_id, NULL, enable_watchdog, (void *) worker->watchdog);
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

