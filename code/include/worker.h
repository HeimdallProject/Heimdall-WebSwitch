//
//============================================================================
// Name       : worker.h
// Description: This is the webswitch single Worker thread body.
// ===========================================================================
//
#ifndef WEBSWITCH_WORKER_H
#define WEBSWITCH_WORKER_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "../include/helper.h"
#include "../include/heimdall_config.h"
#include "../include/watchdog.h"

#define TAG_WORKER "WORKER"
#define WORK_OVER  42                             // so long and thanks for all the fish!

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct worker
 * Description  :
 *
 * Data:
 *
 * Functions:
 * ---------------------------------------------------------------------------
 */
typedef struct worker {
	pid_t worker_id;

	pthread_t watch_thread;                        // thread watchdog identifier
    pthread_t writer_thread;                       // thread writer   identifier
    pthread_t reader_thread;                       // thread reader   identifier

    int writer_thread_status;                      // thread writer operation status
    int reader_thread_status;                      // thread reader operation status

    pthread_cond_t await_cond;                     // condition to await thread execution
    pthread_mutex_t await_mtx;                     // mtx for the above condition
    int worker_await_flag;

    int sockfd;

    RequestQueuePtr requests_queue;                // pointer to the queue of the pending requests
    WatchdogPtr watchdog;                          // pointer to the watchdog
} Worker, *WorkerPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : read_work
 * Description: This function is used by a specific thread to run over a inf.
 *              loop to read from a remote socket specified in Worker
 *
 * Param      : void pointer necessary - WorkerPtr in this case
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
void *read_work(void *arg);

/*
 * ---------------------------------------------------------------------------
 * Function   : write_work
 * Description: This function is used by a specific thread to run over a inf.
 *              loop to write into the client socket, it is projected to be used
 *              to help the (blocking) read process.
 *
 * Param      : void pointer necessary - WorkerPtr in this case
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
void *write_work(void *arg);


/*
 * ---------------------------------------------------------------------------
 * Function   : start_worker
 * Description: This function is used to launch all the threads and to allocate
 *              all the data structures and buffer necessary to the worker proper
 *              goal.
 *
 * Param      :
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr start_worker(int sockfd);

/*
 * ---------------------------------------------------------------------------
 * Function   : new_worker
 * Description: Method for worker creation.
 *
 * Param      : pid_t as process identifier
 * Return     : WorkPtr
 * ---------------------------------------------------------------------------
 */
WorkerPtr new_worker(pid_t os_id);

#endif //WEBSWITCH_WORKER_H
