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

int unixBuildAddress(const char *path, struct sockaddr_un *addr);

int unixConnect(const char *path, int type);

int unixListen(const char *path, int backlog);

int unixBind(const char *path, int type);

#endif //WEBSWITCH_UNIX_SOCKET_H
