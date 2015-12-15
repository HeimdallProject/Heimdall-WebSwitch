//
//============================================================================
// Name       : scheduler.h
// Description: This file contains all the routines to start a scheduler thread
//              to retrieve a server from the remote cluster to serve the request.
// ===========================================================================
//
#ifndef WEBSWITCH_SCHEDULER_H
#define WEBSWITCH_SCHEDULER_H

#include <time.h>

#include "apache_status.h"
#include "round_robin.h"
#include "server_pool.h"

#define AWARENESS_LEVEL_LOW   0
#define AWARENESS_LEVEL_HIGH  1

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct scheduler_args
 * Description      : This struct represents the arguments necessary to run the
 *                    scheduler properly
 * ---------------------------------------------------------------------------
 */
typedef struct scheduler_args {
    RRobinPtr     rrobin;                                   // Round Robin struct
    ServerPoolPtr server_pool;                              // Server Pool struct

    ServerPtr (*get_server)(RRobinPtr rrobin);              // to retrieve a server
} Scheduler, *SchedulerPtr;


/*
 * ---------------------------------------------------------------------------
 * Function   : get_server
 * Description: This function is used to retrieve a server struct to
 *              a remote server from the cluster and pass its parameters
 *              to the calling function in order to schedule an http_request /
 *              http_response routine with a client.
 *
 * Param      : SchedulerPtr
 * Return     : ServerPtr
 * ---------------------------------------------------------------------------
 */
ServerPtr get_ready_server(RRobinPtr rrobin);

/*
 * ---------------------------------------------------------------------------
 * Function   : apache_score
 * Description: This function perform an Apache Status check and sets a score
 *              necessary to weight the servers for the statefull discipline
 *              (weighted RR)
 *
 * Param      : ServerNodePtr
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr apache_score(ServerNodePtr server);

/*
 * ---------------------------------------------------------------------------
 * Function   : update_server_routine
 * Description: This function is used by a detached thread to update, using
 *              ApacheStatus, a server weight and to reset the RoundRobin
 *              struct.
 *
 * Param      : void pointer necessary - SchedulerPtr in this case
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
void *update_server_routine(void *arg);


/*
 * ---------------------------------------------------------------------------
 * Function   : init_scheduler
 * Description: This function is used to init all scheduler variables
 *
 * Param      : integer, if the scheduler is state aware
 * Return     : SchedulerPtr
 * ---------------------------------------------------------------------------
 */
SchedulerPtr init_scheduler(int awareness_level);


#endif //WEBSWITCH_SCHEDULER_H
