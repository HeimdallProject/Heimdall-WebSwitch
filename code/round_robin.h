//
// Created by Odysseus on 18/08/15.
//

#ifndef WEBSWITCH_ROUND_ROBIN_H
#define WEBSWITCH_ROUND_ROBIN_H

#include "circular.h"

#define MAX_BROKEN_SERV_ROUTINE_BUFFER 320
//TODO: to modify this constrain!! using realloc?

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
