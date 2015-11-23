#include <string.h>
#include "stdlib.h"
#include "stdio.h"

#include "include/log.h"
#include "include/heimdall_config.h"
#include "include/worker.h"


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
    //ConfigPtr config = get_config();
    //printf("test_config: %s",config->pre_fork);

    /* Init Log */
    //LogPtr log = get_log();
    //log->d(TAG_MAIN, "Start main programm");

    // Creazione del thread Pool
    /*ThreadPoolPtr th_pool = init_thread_pool();
    if (th_pool == NULL){
        log->d(TAG_MAIN, "Error in init_thread_pool()");
        exit(EXIT_FAILURE);
    }*/

    /*WorkerPtr wrk = th_pool->get_worker();
    printf("Worker: %p\n", wrk->thread_identifier);

    int i = 0;
    for (i = 0; i < 10000; ++i){
        sleep(1);
        printf(".");
    }*/

    /* WORKER AND WATCHDOG TESTING - no code area! */
    ThrowablePtr worker_throw = start_worker();
    if (get_throwable()->is_an_error(worker_throw))
        fprintf(stdout, "ERROR: %s\n", get_throwable()->stack_trace);

    
    exit(EXIT_SUCCESS);
}