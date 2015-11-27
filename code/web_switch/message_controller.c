#include "../include/message_controller.h"

int receive_fd(){

    struct msghdr msgh;
    struct iovec iov;
    int data, lfd, sfd, fd;
    ssize_t nr;

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
    if (remove(SOCK_PATH) == -1 && errno != ENOENT){
        fprintf(stderr, "remove-%s", SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    //printf("Receiving via stream socket\n");
    lfd = unixListen(SOCK_PATH, 5);

    if (lfd == -1){
        fprintf(stderr, "unix-listen %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sfd = accept(lfd, NULL, NULL);
    if (sfd == -1){
        fprintf(stderr, "accept-listen\n");
        exit(EXIT_FAILURE);
    }

    /* Set 'control_un' to describe ancillary data that we want to receive */

    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(int));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_RIGHTS;

    /* Set 'msgh' fields to describe 'control_un' */

    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    /* Set fields of 'msgh' to point to buffer used to receive (real)
       data read by recvmsg() */

    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);

    msgh.msg_name = NULL;               /* We don't need address of peer */
    msgh.msg_namelen = 0;

    /* Receive real plus ancillary data */

    nr = recvmsg(sfd, &msgh, 0);
    if (nr == -1){
        fprintf(stderr, "recvmsg\n");
        exit(EXIT_FAILURE);
    }

    //printf("recvmsg() returned %ld\n", (long) nr);

    //if (nr > 0)
        //printf("Received data = %d\n", data);
    
    /* Get the received file descriptor (which is typically a different
       file descriptor number than was used in the sending process) */

    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL || cmhp->cmsg_len != CMSG_LEN(sizeof(int))){
        fprintf(stderr, "bad cmsg header / message length\n");
        exit(EXIT_FAILURE);
    }

    if (cmhp->cmsg_level != SOL_SOCKET){
        fprintf(stderr, "cmsg_level != SOL_SOCKET\n");
        exit(EXIT_FAILURE);
    }

    if (cmhp->cmsg_type != SCM_RIGHTS){
        fprintf(stderr, "cmsg_type != SCM_RIGHTS\n");
        exit(EXIT_FAILURE);
    }

    fd = *((int *) CMSG_DATA(cmhp));   
    //fprintf(stderr, "Received fd=%d\n", fd);

    return fd;

    /* Having obtained the file descriptor, read the file's contents and
       print them on standard output */

    /*for (;;) {
        char buf[100];
        ssize_t numRead;

        numRead = read(fd, buf, 100);
        if (numRead == -1){
            fprintf(stderr, "read\n");
            exit(EXIT_FAILURE);
        }

        if (numRead == 0)
            break;

        int w = write(STDOUT_FILENO, buf, numRead);
        printf("%d\n", w);
    }*/

}

void send_fd(int fd){

    struct msghdr msgh;
    struct iovec iov;
    int data, sfd;
    ssize_t ns;

    /* Allocate a char array of suitable size to hold the ancillary data.
       However, since this buffer is in reality a 'struct cmsghdr', use a
       union to ensure that it is aligned as required for that structure. */
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
                        /* Space large enough to hold an 'int' */
    } control_un;
    struct cmsghdr *cmhp;

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

    fprintf(stderr, "Sending fd %d\n", fd);

    /* Set message header to describe ancillary data that we want to send */

    cmhp = CMSG_FIRSTHDR(&msgh);
    cmhp->cmsg_len = CMSG_LEN(sizeof(int));
    cmhp->cmsg_level = SOL_SOCKET;
    cmhp->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmhp)) = fd;

    /* Do the actual send */

    sfd = unixConnect(SOCK_PATH, SOCK_STREAM);
    if (sfd == -1){
        fprintf(stderr, "unixConnect\n");
        exit(EXIT_FAILURE);
    }

    ns = sendmsg(sfd, &msgh, 0);
    if (ns == -1){
        fprintf(stderr, "sendmsg\n");
        exit(EXIT_FAILURE);
    }

    printf("sendmsg() returned %ld\n", (long) ns);
}