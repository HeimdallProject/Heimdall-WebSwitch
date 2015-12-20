#include "../include/request_queue.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : get_size_request_queue
 * Description: This function gets the number of the nodes in the queue.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *
 * Return     : An integer that indicates the number of the nodes into the queue.
 * ---------------------------------------------------------------------------
 */
int get_size_request_queue(RequestQueuePtr self) {
    return self->size;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : get_front_request_queue
 * Description: This function gets the first node in the queue.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *
 * Return     : The pointer to the first RequestNode
 * ---------------------------------------------------------------------------
 */
RequestNodePtr get_front_request_queue(RequestQueuePtr self) {
    return self->front;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : is_empty_request_queue
 * Description: This function gets true or false depending on whether the queue
 *              is empty or not.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *
 * Return     : TRUE if is empty, FALSE otherwise. (see helper.h)
 * ---------------------------------------------------------------------------
 */
int is_empty_request_queue(RequestQueuePtr self) {
    if (self->front == NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function   : dequeue_request_queue
 * Description: This function returs the first element in the queue and
 *              returns it.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *
 * Return     : NULL if there is no node, the RequestNode pointer otherwise.
 * ---------------------------------------------------------------------------
 */
RequestNodePtr dequeue_request_queue(RequestQueuePtr self) {
    // Gets the front of the queue
    RequestNodePtr front = self->get_front(self);

    if (front != NULL) {
        // Gets the second in the queue
        RequestNodePtr next = front->next;

        // Removes link of the second from the "ex" front
        front->next = NULL;

        // Makes the second first
        if (next != NULL) {
            next->previous = NULL;
        } else {
            self->back = NULL;
        }
        self->front = next;

        // Decrease counter size
        self->size--;
    }

    return front;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : enqueue_request_queue
 * Description: This function enqueues a new node.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *   node   : The node to enqueues.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void enqueue_request_queue(RequestQueuePtr self, RequestNodePtr node) {

    if (self->back == NULL) {
        self->front = node;
        self->back = node;
    } else {
        self->back->next = node;
        node->previous = self->back;
        self->back=node;
    }

    // Increase counter size
    self->size++;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_request_queue
 * Description: This function free all the memory bound with RequestQueue,
 *              INCLUDING all the nodes.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void destroy_request_queue(RequestQueuePtr self) {

    // Frees all nodes
    RequestNodePtr node;
    while ((node = self->dequeue(self)) != NULL) {
        node->destroy(node);
    }

    free(self);                // Free all the struct
}

RequestQueuePtr init_request_queue() {

    RequestQueuePtr request_queue = malloc(sizeof(RequestQueue));
    if (request_queue == NULL) {
        get_log()->e(TAG_REQUEST_NODE, "Memory allocation error in new_request_queue!");
        exit(EXIT_FAILURE);
    }

    // Sets "attributes"
    request_queue->size  = 0;
    request_queue->front = NULL;
    request_queue->back  = NULL;

    // Set "methods"
    request_queue->enqueue   = enqueue_request_queue;
    request_queue->dequeue   = dequeue_request_queue;
    request_queue->is_empty  = is_empty_request_queue;
    request_queue->get_front = get_front_request_queue;
    request_queue->get_size  = get_size_request_queue;

    request_queue->destroy   = destroy_request_queue;

    return request_queue;
}