//
//============================================================================
// Name       : round_robin.h
// Description: This file contains an implementation of some different Round
//              Robin scheduling disciplines using the circular buffer
//              implementation.
// ===========================================================================
//

#ifndef WEBSWITCH_ROUND_ROBIN_H
#define WEBSWITCH_ROUND_ROBIN_H

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "../include/helper.h"
#include "../include/circular.h"
#include "../include/server_pool.h"

#define TAG_ROUND_ROBIN "ROUND_ROBIN"

#define WEIGHT_DEFAULT 1

typedef struct round_robin_struct {
    CircularPtr circular;

    ThrowablePtr (*weight)(CircularPtr circular, Server *servers, int server_num);
    ThrowablePtr (*reset)(struct round_robin_struct *rrobin, ServerPoolPtr pool, int server_num);
    Server *(*get_server)(CircularPtr circular);
}RRobin, *RRobinPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : weight_servers
 * Description: This function is used make a pattern for the Round Robin
 *              discipline in order to use a weighted scheduling.
 *
 * Param      :
 *  servers:    pointer to the server structs list
 *  server_num: the number of the servers in the list
 *
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr weight_servers(CircularPtr circular, Server *servers, int server_num);

/*
 * ---------------------------------------------------------------------------
 * Function   : reset_servers
 * Description: This function is used to reset servers and their weights
 *
 * Param      : RoundRobinPtr
 *              ServerPoolPtr
 *              int, number of the servers
 *
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr reset_servers(RRobinPtr rrobin, ServerPoolPtr pool, int server_num);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_server
 * Description: This function is used to retrieve the next available server
 *
 * Param      : CircularPtr
 * Return     : Server pointer
 * ---------------------------------------------------------------------------
 */
Server *get_server(CircularPtr circular);

/*
 * ---------------------------------------------------------------------------
 * Function   : new_rrobin
 * Description: This function is used to initialize the RoundRobin struct
 *
 * Param      :
 *
 * Return     : RoundRobinPtr
 * ---------------------------------------------------------------------------
 */
RRobinPtr new_rrobin();

#endif //WEBSWITCH_ROUND_ROBIN_H
