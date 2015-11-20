#include "../include/http_response.h"

ThrowablePtr get_http_response(HTTPResponsePtr self, char *buffer) {

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
    ThrowablePtr t_head = self->get_response_head(self, head);
    ThrowablePtr t_body = self->get_response_body(self, body);
    // checking for any error
    if (t_head->is_an_error(t_head) || t_body->is_an_error(t_body)) {
        return get_throwable()->create(STATUS_ERROR, "Trying to parse remote server response", "get_response");
    } else {
        return get_throwable()->create(STATUS_OK, NULL, "get_response");
    }
}

ThrowablePtr get_response_head(HTTPResponsePtr self, char *head) {

    // using struct http_request to retrieve headers
    ThrowablePtr t_resp_head = self->response->read_headers(head, self->response, self->http_response_type);
    if (t_resp_head->is_an_error(t_resp_head)) {
        return get_throwable()->create(STATUS_ERROR, "Trying to parse remote server response", "get_response_head");
    } else {
        return get_throwable()->create(STATUS_OK, NULL, "get_response_head");
    }
}

ThrowablePtr get_response_body(HTTPResponsePtr self, char *body) {

    // setting the body attribute
    if (body == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Trying to parse remote server response - no body",
                                       "get_response_body");
    } else {
        self->http_response_body = body;
        return get_throwable()->create(STATUS_OK, NULL, "get_response_body");
    }
}

void destroy_http_response(HTTPResponsePtr self) {

    // destroyng http_request support struct
    self->response->destroy(self->response);
    // freeing memory allocated for the body during message receiving
    free(self->http_response_body);
}

HTTPResponsePtr new_http_response(void) {

    HTTPResponsePtr http = malloc(sizeof(HTTPResponse));
    if (http == NULL) {
        get_log()->e(TAG_HTTP_RESPONSE, "Memory allocation error in new_http_response\n");
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


/* use cases */
/* TO BE TESTED !!! */