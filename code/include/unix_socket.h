//
//============================================================================
// Name       	: unix_socket.h
// Description  : This file contains all function for make a UNIX socket.
// ===========================================================================
//
#ifndef WEBSWITCH_UNIX_SOCKET_H
#define WEBSWITCH_UNIX_SOCKET_H

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../include/macro.h"
#include "../include/helper.h"
#include "../include/throwable.h"

/*
 * ---------------------------------------------------------------------------
 * Function         : unix_connect
 * Description      : Create a UNIX domain socket of type 'type' and connect it
 *  					to the remote address specified by the 'path'.
 *  					Return the socket descriptor on success, or -1 on error
 *
 * Param            :
 *   path           : The string to convert.
 *   type    		: type of socket
 *	 socket_fd		: socket_fd created from socket() inside this function.
 *
 * Return           : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr unix_connect(const char *path, int type, int *socket_fd);

/*
 * ---------------------------------------------------------------------------
 * Function         : unix_listen
 * Description      : Create stream socket, bound to 'path'. Make the socket a listening
 * 						socket, with the specified 'backlog'. Return socket descriptor on
 * 						success, or -1 on error.
 *
 * Param            :
 *   path           : The string to convert.
 *   backlog    	: socket backlog
 *	 socket_fd		: socket_fd created from socket() inside this function.
 *
 * Return           : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr unix_listen(const char *path, int backlog, int *socket_fd);

#endif //WEBSWITCH_UNIX_SOCKET_H
