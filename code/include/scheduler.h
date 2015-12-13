//
//============================================================================
// Name       : scheduler.h
// Description: This file contains all the routines to start a scheduler thread
//              to retrieve a server from the remote cluster to serve the request.
// ===========================================================================
//
#ifndef WEBSWITCH_SCHEDULER_H
#define WEBSWITCH_SCHEDULER_H

#include "apache_status.h"
#include "round_robin.h"
#include "server_pool.h"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct scheduler_args
 * Description      : This struct represents the arguments necessary to run the
 *                    scheduler properly
 * ---------------------------------------------------------------------------
 */
typedef struct scheduler_args {
    RRobinPtr     rrobin;
    ServerPoolPtr server_pool;
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



#endif //WEBSWITCH_SCHEDULER_H
