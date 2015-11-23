#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "include/heimdall_config.h"
#include "include/thread_pool.h"
#include "include/log.h"

#define TAG_MAIN "MAIN"

/*
 * ---------------------------------------------------------------------------
 *
 * MAIN PROGRAM
 *
 * ---------------------------------------------------------------------------
 */
int main() {

    /* Init Config */
    ConfigPtr config = get_config();
    if(config == NULL)
        exit(EXIT_FAILURE);

    /* Init Log */
    LogPtr log = get_log();
    if(log == NULL)
        exit(EXIT_FAILURE);

    /* Init Thread Pool */
    ThreadPoolPtr th_pool = get_thread_pool();
    if(th_pool == NULL)
        exit(EXIT_FAILURE);

    /* Init scheduler */
    // TODO init scheduler

    log->d(TAG_MAIN, "Start main program");
    log->d(TAG_MAIN, "Config started at address %p", config);
    log->d(TAG_MAIN, "Log started at address %p", log);
    log->d(TAG_MAIN, "Thread Pool started at address %p", th_pool);
    //log->d(TAG_MAIN, "Scheduler started at address %p", scheduler);

    int i = 0;
    for (i = 0; i < 15000; ++i){
        usleep(1);
    }

    WorkerPtr wrk = th_pool->get_worker();
    printf("Worker: %p\n", wrk);

    i = 0;
    for (i = 0; i < 10000; ++i){
        usleep(1);
    }

    printf("Send signal \n");
    kill(wrk->worker_id, SIGCONT);

    i = 0;
    for (i = 0; i < 10000; ++i){
        usleep(1);
    }

    exit(EXIT_SUCCESS);
}