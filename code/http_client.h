//
//============================================================================
// Name             : http_client.c
// Author           : Claudio Pastorini and Alessio Moretti
// Version          : 0.1
// Data Created     : 07/10/2015
// Last modified    : 12/10/2015
// Description      : This header file contains all the stuffs useful in order to
//                    create a simple HTTP Client.
// ===========================================================================
//
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#define TAG_HTTP_CLIENT "HTTP_CLIENT"  // Macro for log message

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct http_client
 * Description      : This struct collect all attributes and functions pointers to
 *                    manage an HTTP client which forwards request to remote servers
 *                    and wait responses to return them to the calling thread
 * ---------------------------------------------------------------------------
 */
typedef struct http_client {
    struct http_client *self;
    struct http_request *request;                                // the http_request struct to manage connections
    // TODO: create struct http_response check also in http_client
    //struct http_response *response;

    Throwable *(*set_request)(struct http_client *self, struct http_request *req);
    struct http_request *(*get_request)(struct http_client *self, void);
    Throwable *(*make_request)(struct http_client *self, void);
    //Throwable *(*get_response)(struct http_client *self);
    void (*destroy)(void *self);
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

#endif