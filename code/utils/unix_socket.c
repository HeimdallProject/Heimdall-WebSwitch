#include "../include/unix_socket.h"

/*
 * ---------------------------------------------------------------------------
 * Function         : unix_build_address
 * Description      : Build a UNIX domain socket address structure for 'path', returning
 *                      it in 'addr'. Returns -1 on success, or 0 on error.
 *
 * Param            :
 *   path           : The string to convert.
 *   sockaddr_un    : struct sockaddr_un *addr
 *
 * Return           : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr unix_build_address(const char *path, struct sockaddr_un *addr) {

    if (addr == NULL || path == NULL || strlen(path) >= sizeof(addr->sun_path) - 1) {
        errno = EINVAL;
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "unix_build_address");
    }

    memset(addr, 0, sizeof(struct sockaddr_un));
    addr->sun_family = AF_UNIX;
    
    if (strlen(path) < sizeof(addr->sun_path)) { 
        strncpy(addr->sun_path, path, sizeof(addr->sun_path) - 1);
        return get_throwable()->create(STATUS_OK, NULL, "unix_build_address");
    } else {
        errno = ENAMETOOLONG;
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "unix_build_address");
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function         : unix_passive_socket
 * Description      : Create a UNIX domain socket and bind it to 'path'. If 'doListen'
 *                      is true, then call listen() with specified 'backlog'.
 *                      Return the socket descriptor on success, or -1 on error.
 *                      Public interfaces: unix_listen()
 *
 * Param            :
 *   path           : The string to convert.
 *   type           : type of socket
 *   doListen       : If true enable listen() function.
 *   backlog        : socket backlog
 *   value          : socket_fd created from socket() inside this function.
 *
 * Return           : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
static ThrowablePtr unix_passive_socket(const char *path, int type, int doListen, int backlog, int *value) {

    int sd, savedErrno;
    struct sockaddr_un addr;
    ThrowablePtr throwable;

    throwable = unix_build_address(path, &addr);
    if (throwable->is_an_error(throwable)){
        return throwable->thrown(throwable, "unix_build_address");
    } 

    sd = socket(AF_UNIX, type, 0);
    if (sd == -1)
        return get_throwable()->create(STATUS_ERROR, "socket", "unix_passive_socket");

    if (bind(sd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        savedErrno = errno;
        close(sd);                      /* Might change 'errno' */
        errno = savedErrno;
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "bind");
    }

    if (doListen) {
        if (listen(sd, backlog) == -1) {
            savedErrno = errno;
            close(sd);                  /* Might change 'errno' */
            errno = savedErrno;
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "listen");
        }
    }

    *value = sd;

    return get_throwable()->create(STATUS_OK, NULL, "unix_passive_socket");
}

/*
 *  See .h for more information.
 */
ThrowablePtr unix_listen(const char *path, int backlog, int *value) {

    ThrowablePtr throwable = unix_passive_socket(path, SOCK_STREAM, TRUE, backlog, value);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "unix_passive_socket");
    } 

    return get_throwable()->create(STATUS_OK, NULL, "unix_listen");
}

/*
 *  See .h for more information.
 */
ThrowablePtr unix_connect(const char *path, int type, int *value) {

    int sd, savedErrno;
    struct sockaddr_un addr;
    
    ThrowablePtr throwable = unix_build_address(path, &addr);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "unix_build_address");
    } 

    sd = socket(AF_UNIX, type, 0);
    if (sd == -1)
        return get_throwable()->create(STATUS_ERROR, "socket", "unix_connect");

    if (connect(sd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        savedErrno = errno;
        close(sd);                      /* Might change 'errno' */
        errno = savedErrno;
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "unix_connect.connect");
    }

    *value = sd;

    return get_throwable()->create(STATUS_OK, NULL, "unix_connect");
}
