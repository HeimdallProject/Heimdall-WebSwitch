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

    // Sets SO_REUSEADDR and SO_REUSEPORT
    int option = 1;
    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&option, sizeof(option)) == -1) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "create_server_socket");
    }

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
            if (strcpy(ip, addrstr) != ip)
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "hostname_to_ip");
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

    get_log()->d(TAG_CONNECTION, "mando %s", request);

    total = (int) strlen(request);
    get_log()->d(TAG_CONNECTION, "send_request total %d", total);
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
    get_log()->d(TAG_CONNECTION, "send_request total %d", total);

    return get_throwable()->create(STATUS_OK, NULL, "send_request");
}

ThrowablePtr send_request_total(int *sockfd, char *request, int total) {
    int bytes, sent;

    get_log()->d(TAG_CONNECTION, "mando %s", request);

    get_log()->d(TAG_CONNECTION, "send_request total %d", total);
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
    get_log()->d(TAG_CONNECTION, "send_request total %d", total);

    return get_throwable()->create(STATUS_OK, NULL, "send_request");
}

ThrowablePtr receive_http_header(int *sockfd, char *header) {

    ssize_t n_read;                     // Last size read
    ssize_t total_read = 0;             // Total size read
    ssize_t size = sizeof(char) * 1024; // Size of response buffer
    char last_read = ' ';               // Last read chars
    int number = 0;

    while (TRUE) {

        // If the last bytes read are "\r\n\r\n" the header is complete, so return
        if (number == 4) {
            break;
        }

        n_read = read(*sockfd, &last_read, 1); // Read one byte at time

        if (last_read == '\n' || last_read == '\r') {
            number++;
        } else {
            number = 0;
        }

        // Copy the response to the header buffer
        if (strcpy(header + total_read, &last_read) != header + total_read) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
        }

        if (n_read == -1) { // There is an error
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
        } else if (n_read == 0) {   // EOF reached
            break;
        }

        // Increase number of bytes read
        total_read = total_read + sizeof(char) * n_read;

        // If memory is full realloc
        if (total_read == size) {
            if (realloc(header, size + sizeof(char) * 1024) == NULL) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
            }
            // Increase size
            size = size + sizeof(char) * 1024;
        }
    }

    get_log()->d(TAG_CONNECTION, "The header is: %s", header);

    return get_throwable()->create(STATUS_OK, NULL, "receive_http_header");
}

ThrowablePtr receive_http_body(int *sockfd, char *body, ssize_t length, pthread_mutex_t *mutex, int *wrote, int *dimen) {

    ssize_t n_read;                     // Last size read
    ssize_t total_read = 0;             // Total size read
    int new_size;
    //ssize_t size = sizeof(char) * 1024; // Size of response buffer
    //ssize_t read_size = 1024;

    while (TRUE) {
        usleep(50000);
        if (*wrote == TRUE) {    
            get_log()->d(TAG_CONNECTION, "receive_http_body lock mutex");
            if (pthread_mutex_lock(mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
            }

            int diff = length - total_read;
            
            if (diff >= 1024) {
                new_size = 1024;
            } else {
                new_size = diff;
                get_log()->d(TAG_CONNECTION, "receive_http_body rialloco %d", new_size);

                if (realloc(body, sizeof(char) * new_size) == NULL) {
                    return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
                }

                *(body + new_size) = '\0';
            }
            n_read = read(*sockfd, body, new_size); 

            get_log()->d(TAG_CONNECTION, "receive_http_body body %s", body);
            get_log()->d(TAG_CONNECTION, "receive_http_body n_read %d", n_read);

            *dimen = n_read;
            // Increase number of bytes read
            total_read = total_read + n_read;

            *wrote = FALSE;

            get_log()->d(TAG_CONNECTION, "receive_http_body total_read %d length %d", total_read, length);

            get_log()->d(TAG_CONNECTION, "receive_http_body unlock mutex");
            if (pthread_mutex_unlock(mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
            }

            if (total_read == length) {
                break;
            }
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "receive_http_body");
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

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */

/*int main() {

    LogPtr log = get_log();

    // Initializes the new http request
    HTTPRequestPtr http_request = new_http_request();

    // Generates a new simple request
    http_request->set_simple_request(http_request, "GET", "/", "HTTP/1.1", "agesciroma97.org");

    // Creates a new client
    int sockfd;
    ThrowablePtr throwable = create_client_socket(TCP, "5.249.149.45", 80, &sockfd);
    if (throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    }

    // Sends the simple request
    char *message;
    http_request->make_simple_request(http_request, &message);
    send_request(&sockfd, message);

    // Prepares in order to receive the response
    char *response = (char *) malloc(sizeof(char) * 1024);
    if (response == NULL) {
        log->e(TAG_CONNECTION, "Allocation error");
    }

    // Receives the response
    throwable = receive_response(&sockfd, response);
    if(throwable->is_an_error(throwable)) {
        log->t(throwable);
        exit(EXIT_FAILURE);
    }

    // Closes the connection
    close_connection(sockfd);

    // Parse the response and put into the
    HTTPResponsePtr http_response = new_http_response();
    http_response->get_http_response(http_response, response);
    // Destroy the object
    http_request->destroy(http_request);

    return EXIT_SUCCESS;
} */
