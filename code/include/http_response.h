//
//============================================================================
// Name       : http_response.h
// Description: Header file for the HTTP response parsing - it includes all
//              the structs and functions useful to parse the header  and body
//              of the HTTP response in order to collect its headers and body
//              to perform the web switching callback feature
// ============================================================================
//
#ifndef WEBSWITCH_HTTP_RESPONSE_H
#define WEBSWITCH_HTTP_RESPONSE_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../include/helper.h"
#include "../include/log.h"
#include "../include/throwable.h"
#include "../include/http_request.h"

#define TAG_HTTP_RESPONSE "HTTP_RESPONSE"

typedef struct http_response {
    struct http_request  *response;                            // http_request struct pointer which handle a basic response retrieval
                                                               // and will be extended into this struct
    int    http_response_type;                                 // setting http_request struct response type for using its methods
    char  *http_response_body;                                 // the body of the message

    ThrowablePtr (*get_http_response)(struct http_response *self, char *buffer);
    ThrowablePtr (*get_response_head)(struct http_response *self, char *head);
    ThrowablePtr (*get_response_body)(struct http_response *self, char *body);
    void (*destroy)(struct http_response *self);
} HTTPResponse, *HTTPResponsePtr;

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
ThrowablePtr get_http_response(HTTPResponsePtr self, char *buffer);

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
ThrowablePtr get_response_head(HTTPResponsePtr self, char *head);

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
ThrowablePtr get_response_body(HTTPResponsePtr self, char *body);

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_http_response
 * Description: This function will be used to free the memory allocated
 *              for the structure
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_http_response(HTTPResponsePtr self);

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
HTTPResponsePtr new_http_response(void);


#endif //WEBSWITCH_HTTP_RESPONSE_H
