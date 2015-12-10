//
//============================================================================
// Name       : request_queue.h
// Description: This header file contains all the stuffs useful in order to
//              create a request_queue.
// ===========================================================================
//
#ifndef WEBSWITCH_REQUEST_QUEUE_H
#define WEBSWITCH_REQUEST_QUEUE_H

#include "../include/request_node.h"
#include "../include/log.h"

#define TAG_REQUEST_QUEUE "REQUEST_QUEUE" // Macro for log message

/*
 * ---------------------------------------------------------------------------
 * Structure   : request_queue
 * Description : This struct allows to create a linked request node.
 *
 * Data:
 *  front  : The front of the queue (first element).
 *  back   : The back of the queue (last element).
 *  size   : The numbers of the element in the queue.
 *  string : The summary of the node.
 *
 * Functions:
 *  enqueue   : Pointer to enqueue function.
 *  dequeue   : Pointer to dequeue function.
 *  is_empty  : Pointer to is_empty function.
 *  get_front : Pointer to get_front function.
 *  get_size  : Pointer to get_size function.
 *  to_string : Pointer to to_string function.
 *  destroy   : Pointer to destroy function.
 */
typedef struct request_queue {
    RequestNodePtr front;
    RequestNodePtr back;
    int size;
    char *string;

    void (*enqueue)(struct request_queue *self, RequestNodePtr node);
    struct request_node*(*dequeue)(struct request_queue *self);
    int (*is_empty)(struct request_queue *self);
    struct request_node*(*get_front)(struct request_queue *self);
    int (*get_size)(struct request_queue *self);

    char *(*to_string)(struct request_queue *self);
    void (*destroy)(struct request_queue *self);
} RequestQueue, *RequestQueuePtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : init_request_queue
 * Description: This function create a new instance of RequestQueue.
 *
 * Param      : None.
 *
 * Return     : The pointer to new instance of RequestQueue.
 * ---------------------------------------------------------------------------
 */
RequestQueuePtr init_request_queue();

#endif //WEBSWITCH_REQUEST_QUEUE_H
