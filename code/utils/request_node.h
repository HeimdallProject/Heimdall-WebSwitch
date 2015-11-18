//
// Created by claudio on 11/17/15.
//

#ifndef WEBSWITCH_REQUEST_NODE_H
#define WEBSWITCH_REQUEST_NODE_H

#define TAG_REQUEST_NODE "REQUEST_NODE"

typedef struct request_node {
    struct request_node *self;
    pthread_t thread_id;                            // thread identifier
    HTTPResponse response;                          //
    time_t request_timeout;                         // timeout request
    struct request_node *next;
    char *string;

    char *(*to_string)(struct request_node *self);
    void (*destroy)(void *self);
} RequestNode, *RequestNodePtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : new_request_node
 * Description: This function create a new instance of RequestNode.
 *
 * Param      :
 *
 * Return     : The pointer to new instance of RequestNode.
 * ---------------------------------------------------------------------------
 */
RequestNode *new_request_node();

#endif //WEBSWITCH_REQUEST_NODE_H
