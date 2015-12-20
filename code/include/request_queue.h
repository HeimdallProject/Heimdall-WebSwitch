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
 * Example usage.
 * ---------------------------------------------------------------------------
 */
/*int main() {

    LogPtr log = get_log();

    // Initializes queue
    RequestQueuePtr queue = init_request_queue();

    // Initializes nodes
    log->i(TAG_REQUEST_QUEUE, "Creo 3 nodi");
    RequestNodePtr node = init_request_node();
    log->i(TAG_REQUEST_QUEUE, "Node: %s", node->to_string(node));
    RequestNodePtr node1 = init_request_node();
    log->i(TAG_REQUEST_QUEUE, "Node1: %s", node->to_string(node));
    RequestNodePtr node2 = init_request_node();
    log->i(TAG_REQUEST_QUEUE, "Node2: %s", node->to_string(node));

    // Adds nodes to queue
    log->i(TAG_REQUEST_QUEUE, "La coda è attualmente vuota %d (1 per TRUE, 0 per FALSE), ha dimensione %d e %s", queue->is_empty(queue), queue->get_size(queue), queue->to_string(queue->self));
    queue->enqueue(queue->self, node);
    log->i(TAG_REQUEST_QUEUE, "Ho aggiunto node %s", queue->to_string(queue->self));
    queue->enqueue(queue->self, node1);
    log->i(TAG_REQUEST_QUEUE, "Ho aggiunto node1 %s", queue->to_string(queue->self));
    queue->enqueue(queue->self, node2);
    log->i(TAG_REQUEST_QUEUE, "Ho aggiunto node2 %s", queue->to_string(queue->self));

    log->i(TAG_REQUEST_QUEUE, "Quindi la coda ha dimensione %d", queue->get_size(queue));

    log->i(TAG_REQUEST_QUEUE, "Sta in testa %s",(queue->get_front(queue))->to_string(queue->get_front(queue)));

    // Makes a dequeue
    queue->dequeue(queue);

    log->i(TAG_REQUEST_QUEUE, "Dopo una dequeu sta in testa %s",(queue->get_front(queue))->to_string(queue->get_front(queue)));
    log->i(TAG_REQUEST_QUEUE, "Quindi la coda non è vuota %d (1 per TRUE, 0 per FALSE), ha dimensione %d e è %s", queue->is_empty(queue), queue->get_size(queue), queue->to_string(queue->self));

    queue->destroy(queue);

    return EXIT_SUCCESS;
}*/
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

    void (*enqueue)(struct request_queue *self, RequestNodePtr node);
    struct request_node*(*dequeue)(struct request_queue *self);
    int (*is_empty)(struct request_queue *self);
    struct request_node*(*get_front)(struct request_queue *self);
    int (*get_size)(struct request_queue *self);

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
