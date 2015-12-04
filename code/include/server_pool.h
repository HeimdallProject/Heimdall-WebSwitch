//
//============================================================================
// Name       : server_pool.h
// Description: This file contains an implementation of a pool for all the
//              remote machines the web switch will balance on during
//              request scheduling. This implementation is not constrained
//              by any scheduler discipline.
// ===========================================================================
//
#ifndef WEBSWITCH_SERVER_POOL_H
#define WEBSWITCH_SERVER_POOL_H

#include <stdlib.h>
#include <unistd.h>

#include "../include/helper.h"
#include "../include/throwable.h"
#include "../include/log.h"

#define TAG_SERVER_POOL "SERVER_POOL"


#define SERVER_STATUS_READY     1
#define SERVER_STATUS_BROKEN   -1

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct server_node
 * Description      : This struct represents a single server node in order to
 *                    manage a pool of remote machines
 * ---------------------------------------------------------------------------
 */
typedef struct server_node {
    char *host_address;                                         // remote machine canonical name
    char *host_ip;                                              // remote machine ip address
    int  port_number;                                           // remote port number
    int  status;                                                // remote machine status
    int  weight;                                                // remote machine computation weight

    struct server_node *next;                                   // next server_node
} ServerNode, *ServerNodePtr;

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct server_pool
 *
 *                  <head> --> <...> --> <old tail> --> + <new node / new tail>
 *
 * Description      : This struct represents a pool of remote machines
 * ---------------------------------------------------------------------------
 */
typedef struct server_pool {
    struct server_node *head;                                   // first server_node
    struct server_node *tail;                                   // last server_ node
    int    num_servers;                                         // number of servers

    void (*add_server)(struct server_pool *pool, struct server_node *new_node);
    void (*del_server)(struct server_pool *pool, struct server_node *del_node);
} ServerPool, *ServerPoolPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : add_server
 * Description: This function is used to add a server node to the pool
 *
 * Param      : ServerNodePtr
 * Return     :
 * ---------------------------------------------------------------------------
 */
void add_server(ServerPoolPtr pool, ServerNodePtr new_node);

/*
 * ---------------------------------------------------------------------------
 * Function   : delete_server
 * Description: This function is used to delete a server node from the pool
 *
 * Param      : ServerNodePtr
 * Return     :
 * ---------------------------------------------------------------------------
 */
void delete_server(ServerPoolPtr pool, ServerNodePtr del_node);


/*
 * ---------------------------------------------------------------------------
 * Function   : init_server_pool
 * Description: This function is used to initialize the server pool
 *
 * Param      :
 * Return     : ServerPoolPtr
 * ---------------------------------------------------------------------------
 */
ServerPoolPtr init_server_pool(void);

#endif //WEBSWITCH_SERVER_POOL_H


