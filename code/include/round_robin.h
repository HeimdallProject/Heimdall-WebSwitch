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

#include "../include/connection.h"
#include "../include/http_request.h"
#include "../include/helper.h"
#include "../include/circular.h"

#define TAG_ROUND_ROBIN "ROUND_ROBIN"

//TODO: to modify this constrain!! using realloc?
#define MAX_BROKEN_SERV_ROUTINE_BUFFER 320

/*
 * ---------------------------------------------------------------------------
 * Function   : broken_server_routine
 * Description: This function is used to inspect an anomaly over a server and
 *              to wait until the server is actually up. It should be run on a
 *              different thread.
 *
 * Param      :
 *              Pointer to the server struct
 *
 * Return     :
 *              STATUS_OK if connection is again up, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int broken_server_routine(Server *server);

/*
 * ---------------------------------------------------------------------------
 * Function   : weighted_servers
 * Description: This function is used make a pattern for the Round Robin
 *              discipline in order to use a weighted scheduling.
 *
 * Param      :
 *  servers:    pointer to the server structs list
 *  server_num: the number of the servers in the list
 *
 * Return     :
 * ---------------------------------------------------------------------------
 */
void weighted_servers(Server *servers, int server_num);


#endif //WEBSWITCH_ROUND_ROBIN_H
