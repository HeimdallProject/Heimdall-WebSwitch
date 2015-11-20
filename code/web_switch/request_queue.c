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
signed char is_empty_request_queue(RequestQueuePtr self) {
    return self->front == NULL;
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

    free(self->string);        // Free space for to_string
    free(self);                // Free all the struct
}

/*
 * ---------------------------------------------------------------------------
 * Function   : to_string_request_queue
 * Description: This function return a char pointer with a human form of the
 *              RequestQueue.
 *
 * Param      :
 *   self   : The pointer to the RequestQueue.
 *
 * Return     : The pointer of the string.
 * ---------------------------------------------------------------------------
 */
char *to_string_request_queue(RequestQueuePtr self) {
    char *string;

    if (self->string != NULL) {
        free(self->string);
    }

    string = malloc(sizeof(char) * 64000);  // TODO i do not know how many space
    snprintf(string,
             64000,
             "\nRequest Queue: \n\n"
                     "Front: %p\n"
                     "Back: %p\n",
             self->front,
             self->back);

    self->string = string;  //  Save pointer in the struct in order to free after
    return string;
}

RequestQueuePtr init_request_queue() {

    RequestQueuePtr request_queue = malloc(sizeof(RequestQueue));
    if (request_queue == NULL) {
        get_log()->e(TAG_REQUEST_NODE, "Memory allocation error in new_request_queue.\n");
        exit(EXIT_FAILURE);
    }
    // Set self linking
    request_queue->self = request_queue;

    request_queue->size = 0;
    request_queue->front = NULL;
    request_queue->back = NULL;

    // Set "methods"
    request_queue->enqueue = enqueue_request_queue;
    request_queue->dequeue = dequeue_request_queue;
    request_queue->is_empty = is_empty_request_queue;
    request_queue->get_front = get_front_request_queue;

    request_queue->get_size = get_size_request_queue;

    request_queue->destroy = destroy_request_queue;
    request_queue->to_string = to_string_request_queue;

    return request_queue;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main() {

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
}