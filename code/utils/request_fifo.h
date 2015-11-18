//
// Created by claudio on 11/17/15.
//

#ifndef WEBSWITCH_REQUEST_FIFO_H
#define WEBSWITCH_REQUEST_FIFO_H

#define TAG_REQUEST_FIFO "REQUEST_FIFO"

typedef struct request_fifo {
    int (*add_request)(RequestNodePtr request_node);
    int (*delete_request)(RequestNodePtr request_node);
    int (*search_request)(RequestNodePtr request_node);

    char *(*to_string)(struct request_fifo *self);
    void (*destroy)(void *self);
} RequestFifo, *RequestFifoPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : new_request_fifo
 * Description: This function create a new instance of RequestFifo.
 *
 * Param      :
 *
 * Return     : The pointer to new instance of RequestFifo.
 * ---------------------------------------------------------------------------
 */
RequestFifoPtr init_request_fifo();

#endif //WEBSWITCH_REQUEST_FIFO_H
