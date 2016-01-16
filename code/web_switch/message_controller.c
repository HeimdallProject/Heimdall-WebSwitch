#include "../include/message_controller.h"

ThrowablePtr receive_fd(int *file_descriptor, pid_t worker_id){

    struct msghdr msgh;
    struct iovec iov;
    int data, sfd;
    ssize_t nr;

    int value_lfd = 0;
    ThrowablePtr throwable;

    char *pathname;
    if (asprintf(&pathname, "%s_%ld", "/home/vagrant/sockets/", (long)worker_id) < 0) {
        return get_throwable()->create(STATUS_ERROR, "asprintf", "receive_fd");
    }

    /* Allocate a char array of suitable size to hold the ancillary data.
       However, since this buffer is in reality a 'struct cmsghdr', use a
       union to ensure that it is aligned as required for that structure. */
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold an 'int' */
    } control_un;
    struct cmsghdr *cmhp;

    /* Create socket bound to well-known address */
    if (remove(pathname) == -1 && errno != ENOENT)
        return get_throwable()->create(STATUS_ERROR, "remove", "receive_fd");

    throwable = unix_listen(pathname, 5, &value_lfd);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "unix_listen");
    }

    sfd = accept(value_lfd, NULL, NULL);
    if (sfd == -1)
        return get_throwable()->create(STATUS_ERROR, "accept", "receive_fd");

    /* Set 'control_un' to describe ancillary data that we want to receive */
    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(int));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_RIGHTS;

    /* Set 'msgh' fields to describe 'control_un' */
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    /* Set fields of 'msgh' to point to buffer used to receive (real) data read by recvmsg() */
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);

    /* We don't need address of peer */
    msgh.msg_name = NULL;               
    msgh.msg_namelen = 0;

    /* Receive real plus ancillary data */
    nr = recvmsg(sfd, &msgh, 0);
    if (nr == -1)
        return get_throwable()->create(STATUS_ERROR, "recvmsg", "receive_fd");
    
    /* Get the received file descriptor (which is typically a different
       file descriptor number than was used in the sending process) */
    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL || cmhp->cmsg_len != CMSG_LEN(sizeof(int)))
        return get_throwable()->create(STATUS_ERROR, "bad cmsg header / message length", "receive_fd");

    if (cmhp->cmsg_level != SOL_SOCKET)
        return get_throwable()->create(STATUS_ERROR, "cmsg_level != SOL_SOCKET", "receive_fd");

    if (cmhp->cmsg_type != SCM_RIGHTS)
        return get_throwable()->create(STATUS_ERROR, "cmsg_type != SCM_RIGHTS", "receive_fd");

    *file_descriptor = *((int *) CMSG_DATA(cmhp));

    get_log()->d(TAG_MESSAGE_CONTROLLER, "File descriptor %d received to %ld", *file_descriptor, (long)worker_id);

    return get_throwable()->create(STATUS_OK, NULL, "receive_fd");
}

ThrowablePtr send_fd(int fd, pid_t worker_id){

    struct msghdr msgh;
    struct iovec iov;
    int data;
    ssize_t ns;

    int sfd = 0;
    ThrowablePtr throwable;

    char *pathname;
    if (asprintf(&pathname, "%s_%ld", "/home/vagrant/sockets/", (long)worker_id) < 0) {
        return get_throwable()->create(STATUS_ERROR, "asprintf", "receive_fd");
    }

    /* Allocate a char array of suitable size to hold the ancillary data.
       However, since this buffer is in reality a 'struct cmsghdr', use a
       union to ensure that it is aligned as required for that structure. */
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold an 'int' */
    } control_un;
    struct cmsghdr *cmhp;

    memset(&msgh, 0, sizeof(msgh));
    /* On Linux, we must transmit at least 1 byte of real data in
       order to send ancillary data */
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);
    data = 12345;

    /* We don't need to specify destination address, because we use
       connect() below */
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;

    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    /* Set message header to describe ancillary data that we want to send */
    cmhp = CMSG_FIRSTHDR(&msgh);
    cmhp->cmsg_len = CMSG_LEN(sizeof(int));
    cmhp->cmsg_level = SOL_SOCKET;
    cmhp->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmhp)) = fd;

    /* Do the actual send */
    throwable = unix_connect(pathname, SOCK_STREAM, &sfd);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "unix_connect");
    }

    ns = sendmsg(sfd, &msgh, 0);
    if (ns == -1)
        return get_throwable()->create(STATUS_ERROR, "sendmsg", "send_fd");

    get_log()->d(TAG_MESSAGE_CONTROLLER, "File descriptor %d sent to %ld", fd, (long)worker_id);

    return get_throwable()->create(STATUS_OK, NULL, "send_fd");
}