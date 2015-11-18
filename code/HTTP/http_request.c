//
//============================================================================
// Name       : http_request.c
// Author     : Alessio Moretti
// Version    : 0.1
// Description: Code file for the HTTP request header parsing - it includes all
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/errno.h>

#include "http_request.h"
#include "../utils/helper.h"
#include "../utils/log.h"

Throwable *get_header(char *req_line, HTTPRequest *http) {

    char delimiter = ':';
    char *header = NULL;
    char *header_data = NULL;
    int i;
    for (i = 0; i < (int) strlen(req_line); i++) {
        if (req_line[i] == delimiter) {
            req_line[i] = '\0';
            header = str_to_lower(req_line);
            header_data = req_line + i + 2;
            break;
        }
    }

    // check for data presence -> correctly handled req_line retrieval
    if ((header == NULL) || (header_data) == NULL) {
        return NULL;
    }

    // following a comparison between headers which can be useful for perform
    // a request (forwarding packets is the final goal)
    if (strcmp(header, ACCEPT) == 0) {
        http->req_accept = malloc(sizeof(char) * strlen(header_data));
        if (http->req_accept == NULL)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        http->req_accept = header_data;
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, FROM) == 0) {
        http->req_from = malloc(sizeof(char) * strlen(header_data));
        if (http->req_from == NULL)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        http->req_from = header_data;
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, HOST) == 0) {
        http->req_host = malloc(sizeof(char) * strlen(header_data));
        if (http->req_host == NULL)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        http->req_host = header_data;
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, CONTENT_TYPE) == 0) {
        http->req_content_type = malloc(sizeof(char) * strlen(header_data));
        if (http->req_content_type == NULL)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        http->req_content_type = header_data;
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, CONTENT_LEN) == 0) {
        http->req_content_len = malloc(sizeof(char) * strlen(header_data));
        if (http->req_content_len == NULL)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        http->req_content_len = header_data;
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
}

Throwable *get_request(char *req_line, HTTPRequest *http, int len) {

    char delimiter = ' ';

    // allocating the request units
    http->req_type = malloc(sizeof(char) * (REQ_UNIT + 1));
    if (http->req_type == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");

    http->req_protocol = malloc(sizeof(char) * (strlen(PROTOCOL) + 1));
    if (http->req_protocol == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");

    http->req_resource = malloc(sizeof(char) * (len - REQ_UNIT - strlen(PROTOCOL) - 2 + 1));
    if (http->req_resource == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");

    // retrieving request params, not headers
    int counter = 1;
    int start = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (req_line[i] == delimiter) {
            switch (counter) {
                case 1:
                    req_line[i] = '\0';
                    http->req_type = &req_line[start];
                    start = i + 1;
                    counter++;
                    break;
                case 2:
                    req_line[i] = '\0';
                    http->req_resource = &req_line[start];
                    start = i + 1;
                    http->req_protocol = &req_line[start];
                    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
                default:
                    break;
            }
        }
    }

    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
}


Throwable *get_response(char *req_line, HTTPRequest *http, int len) {

    char delimiter = ' ';

    http->resp_code = malloc(sizeof(char) * (REQ_UNIT + 1));
    if (http->resp_code == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_response");

    http->req_protocol = malloc(sizeof(char) * (strlen(PROTOCOL) + 1));
    if (http->req_protocol == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_response");

    http->resp_msg = malloc(sizeof(char) * (len - REQ_UNIT - strlen(PROTOCOL) - 2 + 1));
    if (http->resp_msg == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_response");

    int counter = 1;
    int start = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (req_line[i] == delimiter) {
            switch (counter) {
                case 1:
                    req_line[i] = '\0';
                    http->req_protocol = &req_line[start];
                    start = i + 1;
                    counter++;
                    break;
                case 2:
                    req_line[i] = '\0';
                    http->resp_code = &req_line[start];
                    start = i + 1;
                    http->resp_msg = &req_line[start];
                    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_response");
                default:
                    break;
            }
        }
    }

    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_response");
}


Throwable *read_headers(char *buffer, HTTPRequest *http, int type) {

    char endline = '\n';
    int start = 0;
    int i;
    for (i = 0; buffer[i]; i++) {

        if (buffer[i] == endline) {
            buffer[i] = '\0';
            if (start == 0) {
                switch (type) {
                    case RQST:
                        if (get_request(buffer, http, i)->is_an_error)
                            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
                        break;
                    case RESP:
                        if (get_response(buffer, http, i)->is_an_error)
                            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
                        break;
                    default:
                        break;
                }
                start = i + 1;
            } else {
                if (get_header(buffer + start, http)->is_an_error)
                    return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
                start = i + 1;
            }

            if ((buffer[i + 1] == endline) || (buffer[i + 1] == '\0'))
                break;
        }
    }

    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
}

void set_simple_request(struct http_request *self, char *request_type, char *request_resource, char *request_protocol,
                        char *string) {

    // setting the params for a simple http request
    ((HTTPRequest *) self)->req_type = request_type;
    ((HTTPRequest *) self)->req_protocol = request_protocol;
    ((HTTPRequest *) self)->req_resource = request_resource;
}

Throwable *make_simple_request(void *self, char **result) {

    // checking if params are set
    if (((HTTPRequest *) self)->req_type == NULL     ||
        ((HTTPRequest *) self)->req_protocol == NULL ||
        ((HTTPRequest *) self)->req_resource == NULL) {
        return (*get_throwable()).create(STATUS_ERROR, "Trying to create a simple http request with no params set", "make_simple_request");
    }

    // allocating the memory necessary for the result char buffer
    int result_size = (int) strlen(((HTTPRequest *) self)->req_type)     + 1 +
                      (int) strlen(((HTTPRequest *) self)->req_protocol) + 1 +
                      (int) strlen(((HTTPRequest *) self)->req_resource);
    *result = malloc(sizeof(char) * result_size + 2);
    if (*result == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "make_simple_request)");

    // creating the simple request
    int s = sprintf(*result, "%s %s %s\n\n", ((HTTPRequest *) self)->req_type,
                                          ((HTTPRequest *) self)->req_resource,
                                          ((HTTPRequest *) self)->req_protocol);

    return s < 0 ? (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "make_simple_request") :
                   (*get_throwable()).create(STATUS_OK, NULL, "make_simple_request");
}

void destroy_http_request(void *self) {

    // freeing the memory allocated for all the http request handling
    free(((HTTPRequest *) self)->status);
    free(((HTTPRequest *) self)->req_type);
    free(((HTTPRequest *) self)->req_protocol);
    free(((HTTPRequest *) self)->req_resource);
    free(((HTTPRequest *) self)->req_accept);
    free(((HTTPRequest *) self)->req_from);
    free(((HTTPRequest *) self)->req_host);
    free(((HTTPRequest *) self)->req_content_type);
    free(((HTTPRequest *) self)->req_content_len);
    free(((HTTPRequest *) self)->req_upgrade);
    free(self);
}

HTTPRequest *new_http_request(void) {

    HTTPRequest *http = malloc(sizeof(HTTPRequest));
    if (http == NULL) {
        (*get_log()).e(TAG_HTTP_REQUEST, "Memory allocation error in new_http_request\n");
        exit(EXIT_FAILURE);
    }

    // autoreferencing
    http->self = http;

    // functions pointers initialization
    http->get_header            = get_header;
    http->get_request           = get_request;
    http->read_headers          = read_headers;
    http->make_simple_request   = make_simple_request;
    http->set_simple_request    = set_simple_request;
    http->destroy               = destroy_http_request;

    return http;
}

// use cases
/*int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "Error in usage: %s\n", argv[0]);
    }
    HTTPRequest *http = new_http_request();
    http->set_simple_request(http->self, "POST", "/index.html", "HTTP/1.1");

    char *res;
    http->make_simple_request(http->self, &res);

    return 0;
}*/