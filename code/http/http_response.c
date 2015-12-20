#include "../include/http_response.h"

ThrowablePtr get_http_response(HTTPResponsePtr self, char *buffer) {

    char carriage = '\r';
    char endline = '\n';
    char *head;
    char *body = NULL;

    int i;
    // parsing and dividing into head and body buffers
    head = buffer;
    for (i = 0; i < (signed) strlen(buffer); i++) {
        if (buffer[i] == carriage && buffer[i + 1] == endline) {
            // if it is double CRLF...
            if (buffer[i + 2] == carriage && buffer[i + 3] == endline) {
                // ...dividing headers and body!
                head[i] = '\0';
                body = buffer + i + 4;
                break;
            }
        }
    }

    // letting the other delegate functions parse headers and body
    ThrowablePtr throwable = self->get_response_head(self, head);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "get_http_response");
    } 

    throwable = self->get_response_body(self, body);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "get_http_response");
    } 

    return get_throwable()->create(STATUS_OK, NULL, "get_response");
}

ThrowablePtr get_response_head(HTTPResponsePtr self, char *head) {

    // using struct http_request to retrieve headers
    ThrowablePtr throwable = self->response->read_headers(self->response, head, self->http_response_type);
    if (throwable->is_an_error(throwable)) {
        return get_throwable()->create(STATUS_ERROR, "Trying to parse remote server response", "get_response_head");
    } 

    return get_throwable()->create(STATUS_OK, NULL, "get_response_head");
}


ThrowablePtr get_response_body(HTTPResponsePtr self, char *body) {

    // setting the body attribute
    if (body == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Trying to parse remote server response - no body", "get_response_body");
    } else {
        // allocating
        self->http_response_body = malloc(sizeof(char) * (strlen(body) + 1));
        if (self->http_response_body == NULL) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response_body");
        } else {
            // storing body into struct
            if (strcpy(self->http_response_body, body) != self->http_response_body)
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "get_response_body");
        }
        return get_throwable()->create(STATUS_OK, NULL, "get_response_body");
    }
}

void destroy_http_response(HTTPResponsePtr self) {
    self->response->destroy(self->response);
    free(self->http_response_body);
    free(self);
}

HTTPResponsePtr new_http_response(void) {

    HTTPResponsePtr http = malloc(sizeof(HTTPResponse));
    if (http == NULL) {
        get_log()->e(TAG_HTTP_RESPONSE, "Memory allocation error in new_http_response!");
        exit(EXIT_FAILURE);
    }

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