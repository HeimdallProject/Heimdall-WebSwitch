//
//============================================================================
// Name       	: message_controller.h
// Description	: This program sends a file descriptor to a UNIX domain socket.
// ===========================================================================
//
#ifndef MESSAGE_CONTROLLER_H
#define MESSAGE_CONTROLLER_H

/* To get SCM_CREDENTIALS definition from <sys/sockets.h> */
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "../include/unix_socket.h"

#define SOCK_PATH "/home/vagrant/scm_cred"
#define TAG_MESSAGE_CONTROLLER "MESSAGE_CONTROLLER"

int receive_fd();
int send_fd(int fd);

#endif //MESSAGE_CONTROLLER_H