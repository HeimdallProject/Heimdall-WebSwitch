//
//============================================================================
// Name       : connection.h
// Description: Header file for our client/server project with simple socket's
//              functions.
// ============================================================================
//

#ifndef WEBSWITCH_CONNECTION_H
#define WEBSWITCH_CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/throwable.h"
#include "../include/http_request.h"
#include "../include/http_response.h"
#include "../include/log.h"
#include "../include/macro.h"

#define TAG_CONNECTION "CONNECTION"

#define TCP 0
#define UDP 1

/*
 * Function   : create_server_socket
 * Description: This function creates a TCP or a UDP server
 *              bound at specified port.
 *
 * Param      :
 *   type:    The type of the socket, 0 for TCP, 1 per UDP.
 *   port:    The number of the port where bind the server.
 *   sockfd:  The pointer of the int where save the file
 *            description.
 *
 * Return     : A Throwable.
 */
ThrowablePtr create_server_socket(const int type, const int port, int *sockfd);

/*
 * Function   : create_client_socket
 * Description: This function creates a TCP or a UDP client
 *              that connects itself at specific IP thorough
 *              a specific port.
 *
 * Param      :
 *   type:    The type of the socket, 0 for TCP, 1 per UDP.
 *   port:    The number of the port where bind the server.
 *   sockfd:  The pointer of the int where save the file
 *            description.
 *
 * Return     : A Throwable.
 */
ThrowablePtr create_client_socket(const int type, const char *ip, const int port, int *sockfd);

/*
 * Function   : listen_to
 * Description: This function allows a server to listen to a
 *              specified socket.
 *
 * Param      :
 *   sockfd:  The socket for which listen for.
 *   backlog: Max lenght of connection queue.
 *
 * Return     : A Throwable.
 */
ThrowablePtr listen_to(const int sockfd, const int backlog);

/*
 * Function   : accept_connection
 * Description: This function allows a server to accept a connection
 *              from a specified socket.
 *
 * Param      :
 *   sockfd:     The socket for which accept for.
 *   connection: The pointer of the int where save the new
 *               file description.
 *
 * Return     : A Throwable.
 */
ThrowablePtr accept_connection(const int sockfd, int *connection);

/*
 * Function   : close_connection
 * Description: This function closes the connection.
 *
 * Return     : A Throwable.
 */
ThrowablePtr close_connection(const int connection);

/*
 * Function   : send_request
 * Description: This function sends an HTTP request through a socket.
 *
 * Param      :
 *   sockfd:     The socket where write.
 *   request:    The pointer to the request to send.
 *
 * Return     : A Throwable.
 */
ThrowablePtr send_request(int *sockfd, char *request);

/*
 * Function   : receive_response
 * Description: This function receives an HTTP response through a socket.
 *
 * Param      :
 *   sockfd:     The socket from where read.
 *   request:    The pointer where put the response.
 *
 * Return     : A Throwable.
 */
ThrowablePtr receive_response(int *sockfd, char *response);

#endif //WEBSWITCH_CONNECTION_H