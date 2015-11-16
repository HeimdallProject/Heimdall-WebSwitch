//
//============================================================================
// Name       : http_response.h
// Author     : Alessio Moretti
// Version    : 0.1
// Description: Header file for the HTTP response parsing - it includes all
//              the structs and functions useful to parse the header  and body
//              of the HTTP response in order to collect its headers and body
//              to perform the web switching callback feature
// ============================================================================
//

#ifndef WEBSWITCH_HTTP_RESPONSE_H
#define WEBSWITCH_HTTP_RESPONSE_H

#define TAG_HTTP_RESPONSE "HTTP_RESPONSE"

#include "../utils/throwable.h"
#include "http_request.h"

typedef struct http_response {
    struct http_response *self;                                // autoreferencing the struct
    struct http_request  *response;                            // http_request struct pointer which handle a basic response retrieval
                                                               // and will be extended into this struct
    int    http_response_type;                                 // setting http_request struct response type for using its methods
    char  *http_response_body;                                 // the body of the message

    Throwable *(*get_http_response)(void *self, char *buffer);
    Throwable *(*get_response_head)(void *self, char *head);
    Throwable *(*get_response_body)(void *self, char *body);
    void (*destroy)(void *self);
} HTTPResponse;

/*
 * ---------------------------------------------------------------------------
 * Function   : get_response
 * Description: This function is used to retrieve the response and handle
 *              its parsing
 *
 * Param      :
 *   struct http_response : the autoreferences struct which handles the response
 *   char pointer head    : the pointer to the car buffer from the socket
 *
 * Return     :
 *   struct http_response
 * ---------------------------------------------------------------------------
 */
Throwable *get_http_response(void *self, char *buffer);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_response_head
 * Description: This function is used to retrieve and set the response infos
 *              coming from the remote maching
 *
 * Param      :
 *   struct http_response : the autoreferences struct which handles the response
 *   char pointer head    : the pointer to the headers of the response
 *
 * Return     :
 *   struct http_response
 * ---------------------------------------------------------------------------
 */
Throwable *get_response_head(void *self, char *head);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_response_body
 * Description: This function is used to retrieve the response body, initializing
 *              the struct's own body attribute
 *
 * Param      :
 *   struct http_response : the autoreferences struct which handles the response
 *   char pointer body    : the pointer to the body of the response
 *
 * Return     :
 *   struct http_response
 * ---------------------------------------------------------------------------
 */
Throwable *get_response_body(void *self, char *body);

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_http_response
 * Description: This function will be used to free the memory allocated
 *              for the structure
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_http_response(void *self);

/*
 * ---------------------------------------------------------------------------
 * Function   : new_http_response
 * Description: This function can be used to create the HTTPRequest struct
 *
 * Param      :
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
HTTPResponse *new_http_response(void);


#endif