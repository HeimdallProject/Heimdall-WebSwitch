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

#include "../include/http_request.h"
#include "../include/log.h"
#include "../include/macro.h"

#define TAG_CONNECTION "CONNECTION"

#define TCP 0
#define UDP 1

//TODO create struct for HTTP response
//TODO create wrapper for getnameinfo, better version of gethostbyname
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
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int create_server_socket(const int type, const int port, int *sockfd);

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
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int create_client_socket(const int type, const char *ip, const int port, int *sockfd);

/*
 * Function   : listen_to
 * Description: This function allows a server to listen to a
 *              specified socket.
 *
 * Param      :
 *   sockfd:  The socket for which listen for.
 *   backlog: //TODO: non ho ancora capito che è..
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int listen_to(const int sockfd, const int backlog);

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
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int accept_connection(const int sockfd, int *connection);

/*
 * Function   : close_connection
 * Description: This function close the connection.
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int close_connection(const int connection);

/*
 * Function   : get_server_url
 * Description:
 *
 * Param      :
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int get_server_url(const char *ip, char *url);

/*
 * Function   : send_request
 * Description:
 *
 * Param      :
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int send_request(int *sockfd, HTTPRequest *request);

/*
 * Function   : receive_response
 * Description:
 *
 * Param      :
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 */
int receive_response(int *sockfd);

#endif //WEBSWITCH_CONNECTION_H