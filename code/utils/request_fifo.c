//
// Created by claudio on 11/17/15.
//

#include "request_fifo.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_request_fifo
 * Description: This function free all the memory bound with RequestFifo.
 *
 * Param      :
 *   self   : The pointer to the RequestFifo.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void destroy_request_fifo(RequestFifoPtr self) {
    free(self->string);        // Free space for to_string
    free(self);                // Free all the struct
}

/*
 * ---------------------------------------------------------------------------
 * Function   : to_string_request_fifo
 * Description: This function return a char pointer with a human form of the
 *              RequestFifo.
 *
 * Param      :
 *   self   : The pointer to the RequestFifo.
 *
 * Return     : The pointer of the string.
 * ---------------------------------------------------------------------------
 */
char *to_string_request_fifo(RequestFifoPtr self) {
    char *string;

    if (self->string != NULL) {
        free(self->string);
    }

    char *string = malloc(sizeof(char) * 64000);  // TODO i do not know how many space
    snprintf(string, 64000,
             "Request Fifo: \n\n"
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

RequestFifoPtr new_request_fifo() {

    RequestFifoPtr request_fifo = malloc(sizeof(RequestFifo));
    if (request_fifo == NULL) {
        (*get_log()).e(TAG_REQUEST_NODE, "Memory allocation error in new_request_fifo.\n");
        exit(EXIT_FAILURE);
    }
    // Set self linking
    request_fifo->self = request_fifo;
    request_node->response = "";
    request_node->request_timeout = 10;
    request_node->next = NULL;
    request_fifo->string = NULL;
    // Set "methods"

    request_fifo->destroy = destroy_request_fifo;
    request_fifo->to_string = to_string_request_fifo;

    return request_fifo;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {

    return EXIT_SUCCESS;
}