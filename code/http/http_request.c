
#include "../include/http_request.h"

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
        http->req_accept = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (http->req_accept == NULL              ||
            strcpy(http->req_accept, header_data) != http->req_accept)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, FROM) == 0) {
        http->req_from = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (http->req_from == NULL              ||
            strcpy(http->req_from, header_data) != http->req_from)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, HOST) == 0) {
        http->req_host = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (http->req_host == NULL              ||
            strcpy(http->req_host, header_data) != http->req_host)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, CONTENT_TYPE) == 0) {
        http->req_content_type = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (http->req_content_type == NULL              ||
            strcpy(http->req_content_type, header_data) != http->req_content_type)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
        return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
    }

    if (strcmp(header, CONTENT_LEN) == 0) {
        http->req_content_len = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (http->req_content_len == NULL              ||
            strcpy(http->req_content_len, header_data) != http->req_content_len)
            return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
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
                    if (strcpy(http->req_type, &req_line[start]) != http->req_type)
                        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
                    start = i + 1;
                    counter++;
                    break;
                case 2:
                    req_line[i] = '\0';
                    if (strcpy(http->req_resource, &req_line[start]) != http->req_resource)
                        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
                    start = i + 1;
                    if (strcpy(http->req_protocol, &req_line[start])  != http->req_protocol)
                        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_request");
                    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
                default:
                    break;
            }
        }
    }

    return (*get_throwable()).create(STATUS_OK, NULL, "get_header_http_request");
}


Throwable *get_response(char *req_line, struct http_request *http, int len) {

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
                    if (strcpy(http->req_protocol, &req_line[start]) != http->req_protocol)
                        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_response");
                    start = i + 1;
                    counter++;
                    break;
                case 2:
                    req_line[i] = '\0';
                    if (strcpy(http->resp_code, &req_line[start]) != http->resp_code)
                        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_response");
                    start = i + 1;
                    if (strcpy(http->resp_msg, &req_line[start])  != http->resp_msg)
                        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "get_header_http_response");
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

Throwable *set_simple_request(struct http_request *self, char *request_type, char *request_resource, char *request_protocol, char *host) {

    // allocating params for a simple http request
    self->req_type     = malloc(sizeof(char) * (strlen(request_type)     + 1));
    self->req_protocol = malloc(sizeof(char) * (strlen(request_protocol) + 1));
    self->req_resource = malloc(sizeof(char) * (strlen(request_resource) + 1));
    self->req_host     = malloc(sizeof(char) * (strlen(host)             + 1));

    // checking for memory allocation errors
    if (self->req_type     == NULL ||
        self->req_protocol == NULL ||
        self->req_resource == NULL ||
        self->req_host     == NULL  )
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "set_simple_http_request");

    // setting the params for a simple http request
    if (strcpy(self->req_type,     request_type)     != self->req_type     ||
        strcpy(self->req_protocol, request_protocol) != self->req_protocol ||
        strcpy(self->req_resource, request_resource) != self->req_resource ||
        strcpy(self->req_host,     host)             != self->req_host      )
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "set_simple_http_request");
    else
        return (*get_throwable()).create(STATUS_OK, NULL, "set_simple_http_request");
}

Throwable *make_simple_request(struct http_request *self, char **result) {

    // checking if params are set
    if (self->req_type     == NULL ||
        self->req_protocol == NULL ||
        self->req_resource == NULL ||
        self->req_host     == NULL) {
        return (*get_throwable()).create(STATUS_ERROR, "Trying to create a simple http request with no params set", "make_simple_request");
    }

    // allocating the memory necessary for the result char buffer
    int result_size = (int) strlen(self->req_type)     + 1 +
                      (int) strlen(self->req_protocol) + 1 +
                      (int) strlen(self->req_resource) + 1 +
                      (int) strlen(self->req_host)     + 6 + 3;
    *result = malloc(sizeof(char) * result_size);
    if (*result == NULL)
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "make_simple_request");

    // creating the simple request
    int s = sprintf(*result, "%s %s %s\nHost: %s\n\n",
                    self->req_type,
                    self->req_resource,
                    self->req_protocol,
                    self->req_host);

    return s < 0 ? (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "make_simple_request") :
                   (*get_throwable()).create(STATUS_OK, NULL, "make_simple_request");
}

void destroy_http_request(struct http_request *self) {

    // freeing the memory allocated for all the http request handling
    free(self->status);
    free(self->req_type);
    free(self->req_protocol);
    free(self->req_resource);
    free(self->req_accept);
    free(self->req_from);
    free(self->req_host);
    free(self->req_content_type);
    free(self->req_content_len);
    free(self->req_upgrade);
    free(self);
}

HTTPRequest *new_http_request(void) {

    HTTPRequest *http = malloc(sizeof(HTTPRequest));
    if (http == NULL) {
        return NULL;
    }

    // autoreferencing
    http->self = (void*) http;

    // NULL referencing the attributes
    http->status           = NULL;
    http->req_type         = NULL;
    http->req_protocol     = NULL;
    http->req_resource     = NULL;
    http->req_accept       = NULL;
    http->req_from         = NULL;
    http->req_host         = NULL;
    http->req_content_type = NULL;
    http->req_content_len  = NULL;
    http->req_upgrade      = NULL;

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
int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "Error in usage: %s\n", argv[0]);
    }
    HTTPRequest *http = new_http_request();
    http->set_simple_request(http->self, "GET", "/index.html", "HTTP/1.1", "1.0.0.0");

    char *res;
    http->make_simple_request(http->self, &res);
    fprintf(stdout, "%s\n", res);

    http->destroy(http->self);

    return 0;
}