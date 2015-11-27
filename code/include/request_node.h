//
//============================================================================
// Name       : request_node.h
// Description: This header file contains all the stuffs useful in order to
//              create a node for request_queue.
// ===========================================================================
//
#ifndef WEBSWITCH_REQUEST_NODE_H
#define WEBSWITCH_REQUEST_NODE_H

#include <time.h>
#include <pthread.h>
#include <stdlib.h>

#include "../include/http_response.h"
#include "../include/log.h"

#define TAG_REQUEST_NODE "REQUEST_NODE"

/*
 * ---------------------------------------------------------------------------
 * Structure  : request_node
 * Description: This struct allows to create a linked request node.
 *
 * Data:
 *  self            : Pointer to itself.
 *  pthread_id      : The pthread id bounded to the node.
 *  response        : The response of http request bounded to the node.
 *  request_timeout : The timeout of the request.
 *  previous        : The previous node.
 *  next            : The next node.
 *  string          : The summary of the node.
 *
 * Functions:
 *  to_string  : Pointer to to_string function.
 *  destroy    : Pointer to destroy function.
 */
typedef struct request_node {
    pthread_t thread_id;
    HTTPResponse *response;
    time_t request_timeout;
    struct request_node *previous;
    struct request_node *next;
    char *string;

    char *(*to_string)(struct request_node *self);
    void (*destroy)(struct request_node *self);
} RequestNode, *RequestNodePtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : init_request_node
 * Description: This function create a new instance of RequestNode.
 *
 * Param      : None.
 *
 * Return     : The pointer to new instance of RequestNode.
 * ---------------------------------------------------------------------------
 */
RequestNodePtr init_request_node();

#endif //WEBSWITCH_REQUEST_NODE_H
