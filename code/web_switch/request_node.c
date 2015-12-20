#include "../include/request_node.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_request_node
 * Description: This function free all the memory bound with RequestNode.
 *              It does not free the memory of the previous and the next node.

 *              PAY ATTENTION could be dangerous remove a node without check
 *              next and previous node or other nodes linked.
 *
 * Param      :
 *   self   : The pointer to the RequestNode.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void destroy_request_node(RequestNodePtr self) {
    (self->response)->destroy(self->response);    // Free space of http response
    (self->request)->destroy(self->request);      // Free space of http request
    (self->chunk)->destroy(self->chunk);          // Fress space of chunck
    free(self);                                 // Free all the struct
}

RequestNodePtr init_request_node() {

    RequestNodePtr request_node = malloc(sizeof(RequestNode));
    if (request_node == NULL) {
        get_log()->e(TAG_REQUEST_NODE, "Memory allocation error in init_request_node!");
        exit(EXIT_FAILURE);
    }

    // Initializes condition
    if (pthread_cond_init(&request_node->condition, NULL) != 0) {
        get_log()->e(TAG_REQUEST_NODE, get_error_by_errno(errno));
        exit(EXIT_FAILURE);    
    }

    // Initializes mutex
    if (pthread_mutex_init(&request_node->mutex, NULL) != 0) {
        get_log()->e(TAG_REQUEST_NODE, get_error_by_errno(errno));
        exit(EXIT_FAILURE);
    }

    request_node->thread_id = 1;
    request_node->request = new_http_request();
    request_node->response = new_http_response();
    request_node->request_timeout = 10;
    request_node->previous = NULL;
    request_node->next = NULL;
    request_node->chunk = new_chunk();
    request_node->worker_status = NULL;

    // Sets "methods"
    request_node->destroy = destroy_request_node;

    return request_node;
}