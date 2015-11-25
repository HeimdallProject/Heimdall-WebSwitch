#include <netdb.h>
#include "../include/connection.h"

ThrowablePtr create_socket(const int type, int *sockfd) {

    if (type == 0) {  // TCP
        if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "create_socket");
        }
    } else if (type == 1) {  // UDP
        if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "create_socket");
        }
    } else {
        return get_throwable()->create(STATUS_ERROR, "Usage create_socket_and_bind(<type>), <type> = 0 for TCP <type> = 1 for UDP", "create_socket");
    }

    return get_throwable()->create(STATUS_OK, NULL, "create_socket");
}

ThrowablePtr create_server_socket(const int type, const int port, int *sockfd) {

    ThrowablePtr throwable = create_socket(type, sockfd);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "create_server_socket");
    }

    struct sockaddr_in addr;

    memset((void *) &addr, 0, sizeof(addr));   // Set all memory to 0
    addr.sin_family = AF_INET;                 // Set IPV4 family
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Waiting a connection on all server's IP addresses
    addr.sin_port = htons(port);               // Waiting a connection on PORT

    if (bind(*sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "create_server_socket");
    }

    return get_throwable()->create(STATUS_OK, NULL, "create_server_socket");
}

ThrowablePtr create_client_socket(const int type, const char *ip, const int port, int *sockfd) {

    ThrowablePtr throwable = create_socket(type, sockfd);
    if (throwable->is_an_error(throwable)) return throwable->thrown(throwable, "create_client_socket");

    struct sockaddr_in addr;

    memset((void *) &addr, 0, sizeof(addr));              // Set all memory to 0
    addr.sin_family = AF_INET;                            // Set IPV4 family
    addr.sin_port = (in_port_t) htons((uint16_t) port);   // Set server connection on specified PORT

    if (inet_pton(AF_INET, ip, &addr.sin_addr) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "create_client_socket");
    }

    if (connect(*sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "create_client_socket");
    }

    return get_throwable()->create(STATUS_OK, NULL, "create_client_socket");
}

ThrowablePtr listen_to(const int sockfd, const int backlog) {

    if (listen(sockfd, backlog) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "listen_to");
    }

    return get_throwable()->create(STATUS_OK, NULL, "listen_to");
}

ThrowablePtr hostname_to_ip(char *hostname , char *ip) {
    struct addrinfo *result;

    // Gets info from a hostname
    if (getaddrinfo(hostname, NULL, NULL, &result) != 0) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "hostname_to_ip");
    }

    char addrstr[100];
    void *ptr = NULL;

    while (result) {
        inet_ntop (result->ai_family, result->ai_addr->sa_data, addrstr, 100);

        switch (result->ai_family) {
            case AF_INET:
                ptr = &((struct sockaddr_in *) result->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *) result->ai_addr)->sin6_addr;
                break;
            default:
                return get_throwable()->create(STATUS_ERROR, "Not a valid IP family", "hostname_to_ip");
        }

        // Converts ip address from binary to string form
        if (inet_ntop(result->ai_family, ptr, addrstr, 100) == NULL) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "hostname_to_ip");
        }

        // Removes this lines in order to get all addresses, instead only the first ipv4 is set!
        if (result->ai_family == PF_INET) {
            // Sets result
            ip = addrstr;
            get_log()->d(TAG_CONNECTION, "Host: %s - Ip: %s", hostname, ip);

            // Free resources used
            freeaddrinfo(result);
            return get_throwable()->create(STATUS_OK, NULL, "hostname_to_ip");
        }

        get_log()->d(TAG_CONNECTION, "IPv%d address: %s (%s)\n", result->ai_family == PF_INET6 ? 6 : 4, addrstr, result->ai_canonname);
        result = result->ai_next;
    }

    // Free resources used
    freeaddrinfo(result);

    return get_throwable()->create(STATUS_OK, NULL, "hostname_to_ip");
}

ThrowablePtr accept_connection(const int sockfd, int *connection) {

    if ((*connection = accept(sockfd, (struct sockaddr *)NULL, NULL)) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "accept_connection");
    }

    return get_throwable()->create(STATUS_OK, NULL, "accept_connection");
}

ThrowablePtr close_connection(const int connection) {

    if (close(connection) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "close_connection");
    }

    return get_throwable()->create(STATUS_OK, NULL, "close_connection");
}

ThrowablePtr send_request(int *sockfd, char *request) {
    int bytes, sent, total;

    get_log()->d(TAG_CONNECTION, request);

    total = (int) strlen(request);
    sent = 0;
    do {
        bytes = (int) write(*sockfd, request + sent, (size_t) (total-sent));
        if (bytes < 0) {
            return get_throwable()->create(STATUS_ERROR, "Writing message to socket", "send_request");
        }
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    return get_throwable()->create(STATUS_OK, NULL, "send_request");
}

ThrowablePtr receive_response(int *sockfd, char *response) {

    ssize_t n_read;                     // Last size read
    ssize_t total_read = 0;             // Total size read
    ssize_t size = sizeof(char) * 1024; // Size of response buffer

    while (TRUE) {
        n_read = read(*sockfd, response + total_read, 1); // Read one char at time

        if (n_read == -1) { // There is an error
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_response");
        } else if (n_read == 0) {   // EOF reached
            break;
        }

        // Increase number of bytes read
        total_read = total_read + sizeof(char) * n_read;

        // If memory is full realloc
        if (total_read == size) {
            if (realloc(response, size + sizeof(char) * 1024) == NULL) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_response");
            }
            // Increase size
            size = size + sizeof(char) * 1024;
        }
    }

    get_log()->d(TAG_CONNECTION, response);

    return get_throwable()->create(STATUS_OK, NULL, "receive_response");
}