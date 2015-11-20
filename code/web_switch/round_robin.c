//
//============================================================================
// Name             : round_robin.c
// Author           : Alessio Moretti
// Version          : 0.1
// Date created     : 18/08/2015
// Last modified    : 18/08/2015
// Description      : This file contains an implementation of some different Round
//                    Robin scheduling disciplines using the circular buffer implementation
// ===========================================================================
//

#include "../include/round_robin.h"

// server broken reentering routine
// TODO: to verify the effective necessity of this function and its further implementation
int broken_server_routine(Server *server) {
    // creating a TCP client socket
    int socket_fd;
    if (create_client_socket(0, server->address, server->port, &socket_fd) == STATUS_ERROR) {
        (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "client_socket in broken_server");
        return STATUS_ERROR;
    }

    // making a simple GET request
    char *request;
    /*
    HTTPRequest *http_request = new_http_request();
    http_request->set_simple_request(http_request->self, "GET", "/", "HTTP/1.1");
    http_request->make_simple_request(http_request->self, &request);
    fprintf(stdout, "requesting...");
    fflush(stdout);
    // initializing an HTTP response struct
    HTTPRequest *http_response = new_http_request(); */

    // writing the request into the socket
    int nwrite = (int) strlen(request);
    int writen;
    while ((writen = (int) write(socket_fd, request, (size_t) nwrite)) <= nwrite) {
        if (writen < 0) {
            (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "write in socket in broken_server");
            return STATUS_ERROR;
        }
        if (writen == 0) break;

        nwrite -= writen;
        request += writen;
    }

    // reading the response from the socket
    char *buffer = malloc(sizeof(char)*MAX_BROKEN_SERV_ROUTINE_BUFFER);
    if (buffer == NULL) {
        (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "malloc in broken_server");
        return STATUS_ERROR;
    }

    int nread = MAX_BROKEN_SERV_ROUTINE_BUFFER;
    int readn = 0;
    int n;
    while(nread >= 0) {
        n = (int) read(socket_fd, buffer + readn, (size_t) nread);
        if (n <  0) {
            (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "read from socket in broken_server");
            pthread_exit(NULL);
        }

        readn += n;
        nread -= n;

        if (n == 0 || nread == 0) break;

    }
    buffer[readn] = '\0';

    // reading the response
    /*
    http_response->read_headers(buffer, http_response, RESP);
    if (strcmp(http_response->resp_code, "500") == 0) return STATUS_ERROR;
     */

    return STATUS_OK;
}

// making a weighted scheduling discipline
void weighted_servers(Server *servers, int server_num) {
    // finding the sum for all the servers and sorting them
    int i, j;
    int weight_sum = 0;
    Server temp_serv;
    for (i = 0; i < server_num - 1; i++) {
        weight_sum += (servers + i)->weight;
        for (j = i + 1; j < server_num; j++) {
            if ((servers + i)->weight > (servers + j)->weight) {
                temp_serv = *(servers + i);
                *(servers + i) = *(servers + j);
                *(servers + j) = temp_serv;
            }
        }
    }
    weight_sum += (servers + server_num - 1)->weight;

    // allocating the server pattern sequence
    Server *w_servers = malloc(sizeof(Server)*weight_sum);
    if (w_servers == NULL) {
        (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "malloc in weighted_servers");
        exit(EXIT_FAILURE);
    }

    // creating the pattern
    for (i = 0; i < weight_sum; i++) {
        *(w_servers + i) = *(servers + (i % server_num));
    }

    // creating the circular buffer
    get_circular()->allocate_buffer(&w_servers, weight_sum);
}