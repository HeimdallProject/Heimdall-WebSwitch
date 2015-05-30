//
//============================================================================
// Name       : connection.c
// Author     : Claudio Pastorini
// Version    : 0.2
// Description: Code file for our client/server project with simple socket's
//              functions.
// ============================================================================
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "helper.h"
#include "connection.h"

/*
 * Function   : create_socket
 * Description: This function creates a TCP or UDP socket.
 *
 * Param      :
 *   type:    The type of the socket, 0 for TCP, 1 per UDP.
 *   sockfd:  The pointer of the int where save the file
 *            description.
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int create_socket(const int type, int *sockfd) {

    if (type == 0) {  // TCP
        if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            // TODO manage errno perror("Error in socket()");
            return STATUS_ERROR;
        }
    } else if (type == 1) {  // UDP
        if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            // TODO manage errno perror("Error in socket()");
            return STATUS_ERROR;
        }
    } else {
        fprintf(stderr, "Usage create_socket_and_bind(<type>), <type> = 0 for TCP <type> = 1 for UDP\n");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int create_server_socket(const int type, const int port, int *sockfd) {

    if (create_socket(type, sockfd) == STATUS_ERROR) return STATUS_ERROR;

    struct sockaddr_in addr;

    memset((void *) &addr, 0, sizeof(addr));   // Set all memory to 0
    addr.sin_family = AF_INET;                 // Set IPV4 family
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Waiting a connection on all server's IP addresses
    addr.sin_port = htons(port);               // Waiting a connection on PORT

    if (bind(*sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        // TODO manage errno perror("Error in bind()");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int create_client_socket(const int type, const char *ip, const int port, int *sockfd) {

    if (create_socket(type, sockfd) == STATUS_ERROR) return STATUS_ERROR;

    struct sockaddr_in addr;

    memset((void *) &addr, 0, sizeof(addr));              // Set all memory to 0
    addr.sin_family = AF_INET;                            // Set IPV4 family
    addr.sin_port = (in_port_t) htons((uint16_t) port);   // Set server connection on specified PORT

    if (inet_pton(AF_INET, ip, &addr.sin_addr) == -1) {
        // TODO manage errno perror("Error in inet_pthon()");
        return STATUS_ERROR;
    }

    if (connect(*sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        // TODO manage errno perror("Error in connect()");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int listen_to(const int sockfd, const int backlog) {

    if (listen(sockfd, backlog) == -1) {
        // TODO manage errno perror("Error in listen()");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int accept_connection(const int sockfd, int *connection) {

    if ((*connection = accept(sockfd, (struct sockaddr *)NULL, NULL)) == -1) {
        // TODO manage errno perror("Error in accept()");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int close_connection(const int connection) {

    if (close(connection) == -1) {
        // TODO manage errno perror("Error in close()");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/*
int get_server_url(const char *ip, char *url) {
// TODO complete
    char *host, service;

    */
/*if (getnameinfo() != 0) {
        // TODO manage error that are listed in man
        return STATUS_ERROR;
    }*//*



    return STATUS_OK;
}*/