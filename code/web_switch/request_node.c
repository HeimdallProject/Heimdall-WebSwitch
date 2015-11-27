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
    free(self->string);        // Free space for to_string
    //TODO or not todo self->response->destroy(self->response)
    free(self);                // Free all the struct
}

/*
 * ---------------------------------------------------------------------------
 * Function   : to_string_request_node
 * Description: This function return a char pointer with a human form of the
 *              RequestNode.
 *
 * Param      :
 *   self   : The pointer to the RequestNode.
 *
 * Return     : The pointer of the string.
 * ---------------------------------------------------------------------------
 */
char *to_string_request_node(RequestNodePtr self) {
    char *string;

    if (self->string != NULL) {
        free(self->string);
    }

    string = malloc(sizeof(char) * 64000);  // TODO i do not know how many space

/*    char *next_string;
    if (self->next == NULL) {
        next_string = "null";
    } else {
        next_string = self->next->to_string(self->next);
    }*/

    snprintf(string,   // TODO i do not know how many space
             64000,
             "\nRequest Node: \n\n"
                     "Thread Id: %u\n"
                     "ResponseBody: %s\n"
                     "Timeout: %f\n"
                     "Previous: %p\n"
                     "Next: %p\n",

             (unsigned int) self->thread_id,
             self->response->http_response_body,
             (double) self->request_timeout,
             self->previous,
             self->next);


    self->string = string;  //  Save pointer in the struct in order to free after
    return string;
}

RequestNodePtr init_request_node() {

    RequestNodePtr request_node = malloc(sizeof(RequestNode));
    if (request_node == NULL) {
        get_log()->e(TAG_REQUEST_NODE, "Memory allocation error in init_request_node.\n");
        exit(EXIT_FAILURE);
    }

    request_node->thread_id = 1;        //TODO choose better default value and add other methods
    request_node->response = new_http_response();
    request_node->request_timeout = 10;
    request_node->previous = NULL;
    request_node->next = NULL;
    request_node->string = NULL;

    // Set "methods"
    request_node->destroy = destroy_request_node;
    request_node->to_string = to_string_request_node;

    return request_node;
}