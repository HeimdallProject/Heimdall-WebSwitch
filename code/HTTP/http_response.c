//
//============================================================================
// Name       : http_response.h
// Author     : Alessio Moretti
// Version    : 0.1
// Description: Code file for the HTTP response parsing - it includes all
//              the functions useful to parse the header  and body of the HTTP
//              response in order to collect its headers and body to perform the
//              web switching callback feature
// ============================================================================
//

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/helper.h"
#include "../utils/log.h"
#include "http_response.h"


Throwable *get_http_response(void *self, char *buffer) {

    char endline = '\n';
    char *head;
    char *body = NULL;

    int start = 0;
    int i = 0;

    // parsing and dividing into head and body buffers
    head = buffer + sizeof(char) * start;
    for (i=0; i < (signed) strlen(buffer); i++) {
        if (buffer[i + 1] == endline) {
            head[i + 1] = '\0';
            if (i + 2 < (signed) strlen(buffer)) if (buffer + (sizeof(char) * (i + 2)) != '\0') {
                body = buffer + sizeof(char) * (i + 2);
                break;
            }
        }
    }

    // letting the other delegate functions parse headers and body
    Throwable *t_head = ((HTTPResponse *) self)->get_response_head(self, head);
    Throwable *t_body = ((HTTPResponse *) self)->get_response_body(self, body);
    // checking for any error
    if (t_head->is_an_error || t_body->is_an_error)
        return (*get_throwable()).create(STATUS_ERROR, "Trying to parse remote server response", "get_response");
    else
        return (*get_throwable()).create(STATUS_OK, NULL, "get_response");
}

Throwable *get_response_head(void *self, char *head) {

    // using struct http_request to retrieve headers
    Throwable *t_resp_head = ((HTTPResponse *) self)->response->read_headers(head,
                                                                            ((HTTPResponse *) self)->response,
                                                                            ((HTTPResponse *) self)->http_response_type);
    if (t_resp_head->is_an_error)
        return (*get_throwable()).create(STATUS_ERROR, "Trying to parse remote server response", "get_response_head");
    else
        return (*get_throwable()).create(STATUS_OK, NULL, "get_response_head");
}

Throwable *get_response_body(void *self, char *body) {

    // setting the body attribute
    if (body == NULL)
        return (*get_throwable()).create(STATUS_ERROR, "Trying to parse remote server response - no body", "get_response_body");
    else {
        ((HTTPResponse *) self)->http_response_body = body;
        return (*get_throwable()).create(STATUS_OK, NULL, "get_response_body");
    }
}

void destroy_http_response(void *self) {

    // destroyng http_request support struct
    ((HTTPResponse *) self)->response->destroy(
    ((HTTPResponse *) self)->response->self);
    // freeing memory allocated for the body during message receiving
    free(((HTTPResponse *) self)->http_response_body);
}

HTTPResponse *new_http_response(void) {

    HTTPResponse *http = malloc(sizeof(HTTPResponse));
    if (http == NULL) {
        (*get_log()).e(TAG_HTTP_RESPONSE, "Memory allocation error in new_http_response\n");
        exit(EXIT_FAILURE);
    }

    // autoreferencing
    http->self = http;

    // setting type attribute and body to null
    http->http_response_type = RESP;
    http->http_response_body = NULL;

    // allocating the http_request support struct
    http->response = new_http_request();

    // functions pointers initialization
    http->get_http_response = get_http_response;
    http->get_response_head = get_response_head;
    http->get_response_body = get_response_body;
    http->destroy           = destroy_http_response;

    return http;
}


/* use cases */
/* TO BE TESTED !!! */