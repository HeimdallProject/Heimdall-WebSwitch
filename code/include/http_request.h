//
//============================================================================
// Name       : http_request.h
// Author     : Alessio Moretti
// Version    : 0.1
// Description: Header file for the HTTP request header parsing - it includes all
//              the structs and functions useful to parse the header of the HTTP
//              request in order to let the web switch balancing algorithm to
//              decide the  machines load.
//              Further infos at:
//              - http://tools.ietf.org/html/rfc7230
//              - http://tools.ietf.org/html/rfc7231
//              - http://tools.ietf.org/html/rfc7232
//              - http://tools.ietf.org/html/rfc7233
//              - http://tools.ietf.org/html/rfc7234
//              - http://tools.ietf.org/html/rfc7235
//              - http://en.wikipedia.org/wiki/List_of_HTTP_header_fields (dev)
// ============================================================================
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "../include/helper.h"
#include "../include/log.h"
#include "../include/throwable.h"

#define TAG_HTTP_REQUEST "HTTP_REQUEST"

/*
 * this macros will be used to retrieve the necessary info from the HTTP request
 * along as the maximum header field
 */
#define MAX_HEADER              32768
#define HEADER_ALLOC_UNIT       128
#define REQ_UNIT                4
#define RESOURCE_LOC_UNIT       128
#define PROTOCOL                "HTTP/1.1"
#define KNOWN                   1
#define UNKNOWN                 0

#define RQST                    0
#define RESP                    1

/*
 -----------------------------------------------------------------------------
 *
 *                       1.   REQUEST CONTENT MACROS
 *
 -----------------------------------------------------------------------------
 */
#define ACCEPT                  "accept"
#define ACCEPT_CHAR             "accept-aharset"
#define ACCEPT_ENC              "accept-encoding"
#define ACCEPT_LANG             "accept-language"
#define ACCEPT_DATE             "accept-datetime"
#define CONTENT_LEN             "content-length"
#define CONTENT_MD5             "content-md5"
#define CONTENT_TYPE            "content-type"

/*
 -----------------------------------------------------------------------------
 *
 *                        2.   CLIENT MACROS
 *
 -----------------------------------------------------------------------------
 */
#define FROM                    "from"
#define HOST                    "host"
#define USER_AGENT              "user-agent"


/*
 -----------------------------------------------------------------------------
 *
 *                      3.     ROUTING CONTROL MACROS
 *
 -----------------------------------------------------------------------------
 */
#define CACHE_CTRL              "cache-control"
#define CONNECTION              "connection"
#define COOKIE                  "cookie"
#define DATE                    "date"
#define MODIFIED_SINCE          "if-modified-since"
#define MAX_FORWARDS            "max-forwards"

/*
 -----------------------------------------------------------------------------
 *
 *                       4.     NETWORK SECURITY MACROS
 *
 -----------------------------------------------------------------------------
 */
#define UPGRADE                 "upgrade"
#define VIA                     "via"
#define WARNING                 "warning"

#define INTERNAL_ERROR          "500"

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct http_request
 * Description      : This struct collect all attributes and functions pointers to
 *                    manage, retrieving or creating, HTTP requests
 * ---------------------------------------------------------------------------
 */
typedef struct http_request {
    struct http_request *self;                          // autoreferencing the struct
    char *status;                                       // whether the request can be handled
    char *req_type;                                     // type of the request
    char *req_protocol;                                 // accepted only HTTP/1.1
    char *resp_code;
    char *resp_msg;
    char *req_resource;                                 // resource locator
    char *req_accept;                                   // accepting content info
    char *req_from;                                     // client and request generic info
    char *req_host;
    char *req_content_type;                             // content type info
    char *req_content_len;
    char *req_upgrade;                                  // no protocol upgrade are allowed

    Throwable *(*get_header)(char *req_line, struct http_request *http);
    Throwable *(*get_request)(char *req_line, struct http_request *http, int len);
    Throwable *(*read_headers)(char *buffer, struct http_request *http, int type);
    Throwable *(*make_simple_request)(void *self, char **result);
    void (*set_simple_request)(struct http_request *self, char *request_type, char *request_resource, char *request_protocol,
                            char *string);
    void (*destroy)(void *self);
} HTTPRequest;


/*
 * ---------------------------------------------------------------------------
 * Function   : get_header
 * Description: This function is used to find which header line is currently
 *              read and if it is useful for the web switch to analyze its
 *              contents (balancing algorithm purpose)
 *
 * Param      :
 *   string: the line of the HTTP header currently read by the web switch
 *
 * Return     :
 *   struct http_request
 * ---------------------------------------------------------------------------
 */
Throwable *get_header(char *req_line, struct http_request *http);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_request
 * Description: This function is used to find which request has been performed
 *
 * Param      :
 *   string: the line of the HTTP request header currently read by the web switch
 *
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
Throwable *get_request(char *req_line, struct http_request *http, int len);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_response
 * Description: This function is used to find which response has been sent back
 *
 * Param      :
 *   string: the line of the HTTP response header currently read by the web switch
 *
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
Throwable *get_response(char *req_line, HTTPRequest *http, int len);

/*
 * ---------------------------------------------------------------------------
 * Function   : read_request_headers
 * Description: This function will be used to read the request headers.
 *              Pay attention: it is based this HTTP request format:
 *
 *              METHOD RESOURCE HTTP/1.1
 *              HEADER_1: PARAMS
 *              ...
 *              HEADER_N: PARAMS
 *              [ blank line ]
 *
 * Param      :
 *   buffer:  the buffer into which has been stored the request
 *   http_request struct
 *   type:    integer 0 for REQT and 1 for RESP
 *
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
Throwable *read_headers(char *buffer, struct http_request *http, int type);

/*
 * ---------------------------------------------------------------------------
 * Function   : set_simple_request
 * Description: This function will set the params for a very simple
 *              HTTP request using only:
 *
 *              METHOD RESOURCE HTTP/1.1
 *              [ blank line ]
 *
 *
 * Param      :
 *   self:  the HTTPRequest struct pointer
 *   request_type: the method
 *   request_resource: the universal resource locator
 *   request_protocol: the protocol to be used
 *
 * Return     :
 *   Throwable pointer
 * ---------------------------------------------------------------------------
 */
void set_simple_request(struct http_request *self, char *request_type, char *request_resource, char *request_protocol,
                        char *string);

/*
 * ---------------------------------------------------------------------------
 * Function   : make_simple_request
 * Description: This function will use the setted params to make a simple request
 *              allocating the sufficient memory for the result string
 * Param      :
 *   self:  the HTTPRequest struct pointer
 *   result: the pointer to the string will be used for result string
 *           properly formatted
 *
 * Return     :
 *   Throwable pointer
 * ---------------------------------------------------------------------------
 */
Throwable *make_simple_request(void *self, char **result);

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_http_request
 * Description: This function will be used to free the memory allocated
 *              for the structure
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_http_request(void *self);

/*
 * ---------------------------------------------------------------------------
 * Function   : new_http_request
 * Description: This function can be used to create the HTTPRequest struct
 *
 * Param      :
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
HTTPRequest *new_http_request(void);

#endif //HTTP_REQUEST_H