//
//============================================================================
// Name        : connection.h
// Description : Header file for our client/server project with simple socket's
//               functions.
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
#include <pthread.h> 
#include <netdb.h>
#include <errno.h>

#include "../include/throwable.h"
#include "../include/http_request.h"
#include "../include/http_response.h"
#include "../include/log.h"
#include "../include/macro.h"
#include "../include/chunk.h"

#define TAG_CONNECTION "CONNECTION"

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
    // Initializes the new http request
    HTTPRequestPtr http_request = new_http_request();

    // Sets a new simple request
    http_request->set_simple_request(http_request, "GET", "/server-status?auto", "HTTP/1.1", self->url);

    // Resolves ip from hostname
    char ip[16];
    hostname_to_ip(self->url, ip);

    // Creates a new client
    int sockfd;
    ThrowablePtr throwable = create_client_socket(TCP, ip, 80, &sockfd);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Generates the simple request
    char *message;
    throwable = http_request->make_simple_request(http_request, &message);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Sends request
    throwable = send_request(&sockfd, message);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Prepares in order to receive the response
    char *response = (char *) malloc(sizeof(char) * 1024);
    if (response == NULL) {
        get_throwable()->create(STATUS_ERROR, "Allocation error", "retrieve_apache_status");
    }

    // Receives the response
    throwable = receive_response(&sockfd, response);
    if (throwable->is_an_error(throwable)) {
        throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Closes the connection
    close_connection(sockfd);

    // Parse the response
    HTTPResponsePtr http_response = new_http_response();
    http_response->get_http_response(http_response, response);

    //log->i(TAG_CONNECTION, http_response->http_response_body);

    // Destroy the object
    http_request->destroy(http_request);
 */

#define TCP 0
#define UDP 1

/*
 * Function    : create_server_socket
 * Description : This function creates a TCP or a UDP server
 *               bound at specified port.
 *
 * Param       :
 *   type   : The type of the socket, 0 for TCP, 1 per UDP.
 *   port   : The number of the port where bind the server.
 *   sockfd : The pointer of the int where save the file
 *            description.
 *
 * Return      : A Throwable.
 */
ThrowablePtr create_server_socket(const int type, const int port, int *sockfd);

/*
 * Function    : create_client_socket
 * Description : This function creates a TCP or a UDP client
 *              that connects itself at specific IP thorough
 *              a specific port.
 *
 * Param       :
 *   type   : The type of the socket, 0 for TCP, 1 per UDP.
 *   port   : The number of the port where bind the server.
 *   sockfd : The pointer of the int where save the file
 *            description.
 *
 * Return      : A Throwable.
 */
ThrowablePtr create_client_socket(const int type, const char *ip, const int port, int *sockfd);

/*
 * Function    : listen_to
 * Description : This function allows a server to listen to a
 *               specified socket.
 *
 * Param       :
 *   sockfd  : The socket for which listen for.
 *   backlog : Max lenght of connection queue.
 *
 * Return      : A Throwable.
 */
ThrowablePtr listen_to(const int sockfd, const int backlog);

/*
 * Function    : hostname_to_ip
 * Description : This function allows to resolve ip from a hostname.
 *               In this form it returns the first time a IPV4 is found.
 *
 * Param       :
 *   hostname : The hostname to resolve
 *   ip       : The pointer of the string where save the ip.
 *
 * Return      : A Throwable.
 */
ThrowablePtr hostname_to_ip(char *hostname, char *ip);

/*
 * Function    : accept_connection
 * Description : This function allows a server to accept a connection
 *               from a specified socket.
 *
 * Param       :
 *   sockfd     : The socket for which accept for.
 *   connection : The pointer of the int where save the new
 *                file description.
 *
 * Return      : A Throwable.
 */
ThrowablePtr accept_connection(const int sockfd, int *connection);

/*
 * Function    : close_connection
 * Description : This function closes the connection.
 *
 * Param       :
 *   connection : The connection to close.
 *   
 * Return      : A Throwable.
 */
ThrowablePtr close_connection(const int connection);

/*
 * Function    : receive_response
 * Description : This function receives an HTTP response through a socket.
 *
 * Param       :
 *   sockfd : The socket where read.
 *   header : The pointer where put the header.
 *
 * Return      : A Throwable.
 */
ThrowablePtr receive_http_body(int sockfd, char *body, ssize_t length, pthread_mutex_t *mutex, int *wrote, int *dimen);

/*
 * Function    : send_http
 * Description : This function sends HTTP message through a socket.
 *
 * Param       :
 *   sockfd  : The socket where writes message.
 *   message : The pointer to message to send.
 *   total   : The total lenght of message to send.
 *
 * Return      : A Throwable.
 */
ThrowablePtr send_http(int sockfd, char *message, size_t total);

/*
 * Function    : send_http_request
 * Description : This function sends an HTTP request through a socket.
 *
 * Param       :
 *   sockfd       : The socket where writes message.
 *   http_request : The http request to send.
 *
 * Return      : A Throwable.
 */
ThrowablePtr send_http_request(int sockfd, HTTPRequestPtr http_request);

/*
 * Function    : send_http_response_header
 * Description : This function sends an HTTP response header through a socket.
 *
 * Param       :
 *   sockfd        : The socket where writes message.
 *   http_response : The http response to send.
 *
 * Return      : A Throwable.
 */
ThrowablePtr send_http_response_header(int sockfd, HTTPResponsePtr http_response);

/*
 * Function    : send_http_chunks
 * Description : This function sends chunks of HTTP through a socket using thread syncronization.
 *
 * Param       :
 *   sockfd : The socket where writes message.
 *   chunk  : The pointer to chunk.
 *   total  : The total lenght of message to send.
 *
 * Return      : A Throwable.
 */
ThrowablePtr send_http_chunks(int sockfd, ChunkPtr chunk, int total);

/*
 * Function    : receive_http
 * Description : This function receives an HTTP message through a socket.
 *
 * Param       :
 *   sockfd  : The socket where read.
 *   message : The pointer where puts the message read.
 *
 * Return      : A Throwable.
 */
ThrowablePtr receive_http(int sockfd, char **message);

/*
 * Function    : receive_http_header
 * Description : This function receives an HTTP header through a socket.
 *
 * Param       :
 *   sockfd : The socket where read.
 *   header : The pointer where puts the header read.
 *
 * Return      : A Throwable.
 */
ThrowablePtr receive_http_header(int sockfd, char **header);

ThrowablePtr receive_http_header2(int sockfd, HTTPRequestPtr http_request);

/*
 * Function    : receive_http_request
 * Description : This function receives an HTTP request through a socket.
 *
 * Param       :
 *   sockfd       : The socket where read.
 *   http_request : The pointer to http request.
 *
 * Return      : A Throwable.
 */
ThrowablePtr receive_http_request(int sockfd, HTTPRequestPtr http_request);

/*
 * Function    : receive_http_response_header
 * Description : This function receives an HTTP response header through a socket.
 *
 * Param       :
 *   sockfd        : The socket where read.
 *   http_response : The pointer of http response where put the header.
 *
 * Return      : A Throwable.
 */
ThrowablePtr receive_http_response_header(int sockfd, HTTPResponsePtr http_response);

/*
 * Function    : receive_http_response_header
 * Description : This function receives HTTP chunks through a socket using thread syncronization.
 *
 * Param       :
 *   sockfd        : The socket where read.
 *   http_response : The pointer of http response.
 *   chunk         : The pointer of chunk.
 *
 * Return      : A Throwable.
 */
ThrowablePtr receive_http_chunks(int sockfd, HTTPResponsePtr http_response, ChunkPtr chunk);

#endif //WEBSWITCH_CONNECTION_H