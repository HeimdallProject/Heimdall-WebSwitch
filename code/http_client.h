//
// Created by Odysseus on 07/10/15.
//

#ifndef WEBSWITCH_HTTP_CLIENT_H
#define WEBSWITCH_HTTP_CLIENT_H

#include "throwable.h"
#include "http_request.h"
/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct http_client
 * Description      : This struct collect all attributes and functions pointers to
 *                    manage an HTTP client which forwards request to remote servers
 *                    and wait responses to return them to the calling thread
 * ---------------------------------------------------------------------------
 */
typedef struct http_client {
    struct http_client  *self;                                   // autoreferencing the struct
    struct http_request *request;                                // the http_request struct to manage connections

    Throwable *(*set_request)(struct http_request *req);
    struct http_request *(*get_request)(void);
    Throwable *(*make_request)(void);
    // TODO: create struct http_response
    Throwable *(*get_response)(struct http_response *resp);
} HTTPClient;

/*
 * ---------------------------------------------------------------------------
 * Function   : new_http_client
 * Description: This function can be used to create the HTTPClient struct
 *
 * Param      :
 * Return     :
 *   struct http_client pointer
 * ---------------------------------------------------------------------------
 */
HTTPRequest *new_http_client(void);

#endif //WEBSWITCH_HTTP_CLIENT_H
