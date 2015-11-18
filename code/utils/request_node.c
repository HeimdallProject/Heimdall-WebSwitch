//
// Created by claudio on 11/17/15.
//

#include "request_node.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_request_node
 * Description: This function free all the memory bound with RequestNode.
 *
 * Param      :
 *   self   : The pointer to the RequestNode.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void destroy_request_node(RequestNode *self) {
    free(self->string);        // Free space for to_string
    free(self);                  // Free all the struct
}

/*/
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
char *to_string_request_node(RequestNode *self) {
    char *string;

    if (self->string != NULL) {
        free(self->string);
    }

    char *string = malloc(sizeof(char) * 64000);  // TODO i do not know how many space
    snprintf(string, 64000,
             "Request Node: \n\n"
                     "Thread Id: %d\n"
                     "Response: %d\n"
                     "Timeout: %f\n"
                     "Next: %s",

             self->thread_id,
             self->response->to_string(self->response),
             self->request_timeout,
             self->next->to_string(self->next));

    self->string = string;  //  Save pointer in the struct in order to free after
    return string;
}

RequestNode *new_request_node() {

    RequestNode *request_node = malloc(sizeof(RequestNode));
    if (request_node == NULL) {
        (*get_log()).e(TAG_REQUEST_NODE, "Memory allocation error in new_request_node.\n");
        exit(EXIT_FAILURE);
    }
    // Set self linking
    request_node->self = request_node;
    request_node->thread_id = -1;
    request_node->response = "";
    request_node->request_timeout = 10;
    request_node->next = NULL;
    request_node->string = NULL;
    // Set "methods"

    request_node->destroy = destroy_request_node;
    request_node->to_string = to_string_request_node;

    return request_node;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {

    return EXIT_SUCCESS;
}