#include "../include/http_request.h"

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
ThrowablePtr get_header(HTTPRequestPtr self, char *req_line) {

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
        self->req_accept = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (self->req_accept == NULL              ||
            strcpy(self->req_accept, header_data) != self->req_accept)
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_header");
        return get_throwable()->create(STATUS_OK, NULL, "get_header");
    }

    if (strcmp(header, FROM) == 0) {
        self->req_from = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (self->req_from == NULL              ||
            strcpy(self->req_from, header_data) != self->req_from)
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_header");
        return get_throwable()->create(STATUS_OK, NULL, "get_header");
    }

    if (strcmp(header, HOST) == 0) {
        self->req_host = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (self->req_host == NULL              ||
            strcpy(self->req_host, header_data) != self->req_host)
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_header");
        return get_throwable()->create(STATUS_OK, NULL, "get_header");
    }

    if (strcmp(header, CONTENT_TYPE) == 0) {
        self->req_content_type = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (self->req_content_type == NULL              ||
            strcpy(self->req_content_type, header_data) != self->req_content_type)
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_header");
        return get_throwable()->create(STATUS_OK, NULL, "get_header");
    }

    if (strcmp(header, CONTENT_LEN) == 0) {
        char *req_content_len_string = malloc(sizeof(char) * (strlen(header_data) + 1));
        if (req_content_len_string == NULL || strcpy(req_content_len_string, header_data) != req_content_len_string) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_header");
        }

        
        // Converts into integer
        ThrowablePtr throwable = str_to_int(req_content_len_string, &self->req_content_len);
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_header");
        }

        //get_log()->d(TAG_HTTP_REQUEST, "content_len %s len %d", req_content_len_string, self->req_content_len);

        // Frees memory
        free(req_content_len_string);

        return get_throwable()->create(STATUS_OK, NULL, "get_header");
    }

    return get_throwable()->create(STATUS_OK, NULL, "get_header");
}

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
ThrowablePtr get_request(HTTPRequestPtr self, char *req_line, int len) {

    char delimiter = ' ';

    // allocating the request units
    self->req_type = malloc(sizeof(char) * (REQ_UNIT + 1));
    if (self->req_type == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_request");

    self->req_protocol = malloc(sizeof(char) * (strlen(PROTOCOL) + 1));
    if (self->req_protocol == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_request");

    self->req_resource = malloc(sizeof(char) * (len - REQ_UNIT - strlen(PROTOCOL) - 2 + 1));
    if (self->req_resource == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_request");

    // retrieving request params, not headers
    int counter = 1;
    int start = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (req_line[i] == delimiter) {
            switch (counter) {
                case 1:
                    req_line[i] = '\0';
                    if (strcpy(self->req_type, &req_line[start]) != self->req_type)
                        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_request");
                    start = i + 1;
                    counter++;
                    break;
                case 2:
                    req_line[i] = '\0';
                    if (strcpy(self->req_resource, &req_line[start]) != self->req_resource)
                        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_request");
                    start = i + 1;
                    if (strcpy(self->req_protocol, &req_line[start])  != self->req_protocol)
                        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_request");
                    return get_throwable()->create(STATUS_OK, NULL, "get_header_self_request");
                default:
                    break;
            }
        }
    }
    return get_throwable()->create(STATUS_OK, NULL, "get_request");
}

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
ThrowablePtr get_response(HTTPRequestPtr self, char *req_line, int len) {

    char delimiter = ' ';

    self->resp_code = malloc(sizeof(char) * (REQ_UNIT + 1));
    if (self->resp_code == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response");

    self->req_protocol = malloc(sizeof(char) * (strlen(PROTOCOL) + 1));
    if (self->req_protocol == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response");

    self->resp_msg = malloc(sizeof(char) * (len - REQ_UNIT - strlen(PROTOCOL) - 2 + 1));
    if (self->resp_msg == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response");

    int counter = 1;
    int start = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (req_line[i] == delimiter) {
            switch (counter) {
                case 1:
                    req_line[i] = '\0';
                    if (strcpy(self->req_protocol, &req_line[start]) != self->req_protocol)
                        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response");
                    start = i + 1;
                    counter++;
                    break;
                case 2:
                    req_line[i] = '\0';
                    if (strcpy(self->resp_code, &req_line[start]) != self->resp_code)
                        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response");
                    start = i + 1;
                    if (strcpy(self->resp_msg, &req_line[start])  != self->resp_msg)
                        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response");
                    return get_throwable()->create(STATUS_OK, NULL, "get_response");
                default:
                    break;
            }
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "get_response");
}


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
ThrowablePtr read_headers(HTTPRequestPtr self, char *string, int type) {

    char *buffer = malloc(strlen(string));
    if (buffer == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "read_headers");
    }

    if (strcpy(buffer, string) != buffer) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "read_headers");
    }

    char carriage = '\r';
    char endline = '\n';
    int start = 0;
    int i;
    int length = (signed) strlen(buffer);

    //get_log()->d(TAG_HTTP_REQUEST, "length %d", length);


    for (i = 0; i < length; i++) {

        if (buffer[i] == endline) {
            
            buffer[i - 1] = '\0';

            ThrowablePtr throwable;
            if (start == 0) {
                switch (type) {
                    case RQST:
                        throwable = get_request(self, buffer, i);
                        if (throwable->is_an_error(throwable)) {
                            return throwable->thrown(throwable, "read_headers");
                        }
                        break;
                    case RESP:
                        throwable = get_response(self, buffer, i);
                        if (throwable->is_an_error(throwable)) {
                            return throwable->thrown(throwable, "read_headers");
                        }
                        break;
                    default:
                        break;
                }
                start = i + 1;
            } else {
                throwable = get_header(self, buffer + start);
                if (throwable->is_an_error(throwable))
                    return throwable->thrown(throwable, "read_headers");
                start = i + 1;
            }

            if (buffer[i + 1] == carriage && buffer[i + 2] == endline)
                break;
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "read_headers");
}

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
 *   ThrowablePtr pointer
 * ---------------------------------------------------------------------------
 */
ThrowablePtr set_simple_request(HTTPRequestPtr self, char *request_type, char *request_resource, char *request_protocol, char *host) {

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
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "set_simple_http_request");

    // setting the params for a simple http request
    if (strcpy(self->req_type,     request_type)     != self->req_type     ||
        strcpy(self->req_protocol, request_protocol) != self->req_protocol ||
        strcpy(self->req_resource, request_resource) != self->req_resource ||
        strcpy(self->req_host,     host)             != self->req_host      )
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "set_simple_http_request");
    else
        return get_throwable()->create(STATUS_OK, NULL, "set_simple_http_request");
}

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
 *   ThrowablePtr pointer
 * ---------------------------------------------------------------------------
 */
ThrowablePtr make_simple_request(HTTPRequestPtr self, char **result) {

    // checking if params are set
    if (self->req_type     == NULL ||
        self->req_protocol == NULL ||
        self->req_resource == NULL ||
        self->req_host     == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Trying to create a simple http request with no params set", "make_simple_request");
    }

    // allocating the memory necessary for the result char buffer
    int result_size = (int) strlen(self->req_type)     + 1 +
                      (int) strlen(self->req_protocol) + 1 +
                      (int) strlen(self->req_resource) + 1 +
                      (int) strlen(self->req_host)     + 6 + 3;
    *result = malloc(sizeof(char) * result_size);
    if (*result == NULL)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "make_simple_request");

    // creating the simple request
    int s = sprintf(*result, "%s %s %s\nHost: %s\n\n",
                    self->req_type,
                    self->req_resource,
                    self->req_protocol,
                    self->req_host);

    return s < 0 ? get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "make_simple_request") :
                   get_throwable()->create(STATUS_OK, NULL, "make_simple_request");
}

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_http_request
 * Description: This function will be used to free the memory allocated
 *              for the structure
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_http_request(HTTPRequestPtr self) {

    // freeing the memory allocated for all the http request handling
    free(self->status);
    free(self->req_type);
    free(self->req_protocol);
    free(self->req_resource);
    free(self->req_accept);
    free(self->req_from);
    free(self->req_host);
    free(self->req_content_type);
    free(self->req_upgrade);
    free(self);
}

HTTPRequestPtr new_http_request(void) {

    HTTPRequestPtr http = malloc(sizeof(HTTPRequest));
    if (http == NULL) {
        return NULL; // TODO mh?
    }

    // NULL referencing the attributes
    http->status           = NULL;
    http->req_type         = NULL;
    http->req_protocol     = NULL;
    http->req_resource     = NULL;
    http->req_accept       = NULL;
    http->req_from         = NULL;
    http->req_host         = NULL;
    http->req_content_type = NULL;
    http->req_content_len  = -1;
    http->req_upgrade      = NULL;
    http->header           = NULL;

    // functions pointers initialization
    http->get_header            = get_header;
    http->get_request           = get_request;
    http->read_headers          = read_headers;
    http->make_simple_request   = make_simple_request;
    http->set_simple_request    = set_simple_request;
    http->destroy               = destroy_http_request;

    return http;
}