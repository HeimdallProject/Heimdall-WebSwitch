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
#include "helper.h"
#include "http_request.h"

struct http_request *get_header(char *req_line, struct http_request *http) {

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

    if ((header == NULL) || (header_data) == NULL) {
        return NULL;
    }

    if (strcmp(header, ACCEPT) == 0) {
        http->req_accept = malloc(sizeof(char) * strlen(header_data));
        if (http->req_accept == NULL)
            return NULL;
        http->req_accept = header_data;
        return http;
    }

    if (strcmp(header, FROM) == 0) {
        http->req_from = malloc(sizeof(char) * strlen(header_data));
        if (http->req_from == NULL)
            return NULL;
        http->req_from = header_data;
        return http;
    }

    if (strcmp(header, HOST) == 0) {
        http->req_host = malloc(sizeof(char) * strlen(header_data));
        if (http->req_host == NULL)
            return NULL;
        http->req_host = header_data;
        return http;
    }

    if (strcmp(header, CONTENT_TYPE) == 0) {
        http->req_content_type = malloc(sizeof(char) * strlen(header_data));
        if (http->req_content_type == NULL)
            return NULL;
        http->req_content_type = header_data;
        return http;
    }

    if (strcmp(header, CONTENT_LEN) == 0) {
        http->req_content_len = malloc(sizeof(char) * strlen(header_data));
        if (http->req_content_len == NULL)
            return NULL;
        http->req_content_len = header_data;
        return http;
    }

    return http;
}

struct http_request *get_request(char *req_line, struct http_request *http, int len) {

    char delimiter = ' ';

    http->req_type = malloc(sizeof(char) * (REQ_UNIT + 1));
    if (http->req_type == NULL)
        return NULL;

    http->req_protocol = malloc(sizeof(char) * (strlen(PROTOCOL) + 1));
    if (http->req_protocol == NULL)
        return NULL;

    http->req_resource = malloc(sizeof(char) * (len - REQ_UNIT - strlen(PROTOCOL) - 2 + 1));
    if (http->req_resource == NULL)
        return NULL;

    int counter = 1;
    int start = 0;
    int i;
    for (i = 0; i < len; i ++) {
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
                    return http;
                default:
                    break;
            }
        }
    }

    return http;
}

struct http_request *read_request_headers(char *buffer, struct http_request *http) {

    char endline = '\n';
    int start = 0;
    int i;
    for (i = 0; buffer[i]; i++) {

        if (buffer[i] == endline) {
            buffer[i] = '\0';
            if (start == 0) {
                if (get_request(buffer, http, i - start + i) == NULL)
                    return NULL;
                start = i + 1;
            } else {
                fprintf(stdout, "HEADERS: %s\n", buffer + start);
                if (get_header(buffer + start, http) == NULL)
                    return NULL;
                start = i + 1;
            }

            if ((buffer[i + 1] == endline) || (buffer[i + 1] == '\0'))
                break;
        }
    }

    return http;
}


int main(int argc, char *argv[]) {

    //TODO fare codice di prova
    printf("prova request %d, %s", argc, argv[0]);

    return EXIT_SUCCESS;
}