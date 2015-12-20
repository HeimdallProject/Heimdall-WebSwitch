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

    memset((void *) &addr, 0, sizeof(addr));    // Set all memory to 0
    addr.sin_family = AF_INET;                  // Set IPV4 family
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // Waiting a connection on all server's IP addresses
    addr.sin_port = htons(port);                // Waiting a connection on PORT

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

    memset((void *) &addr, 0, sizeof(addr));            // Set all memory to 0
    addr.sin_family = AF_INET;                          // Set IPV4 family
    addr.sin_port = (in_port_t) htons((uint16_t) port); // Set server connection on specified PORT

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
            //get_log()->d(TAG_CONNECTION, "Host: %s - Ip: %s", hostname, ip);

            // Free resources used
            freeaddrinfo(result);
            return get_throwable()->create(STATUS_OK, NULL, "hostname_to_ip");
        }

        //get_log()->d(TAG_CONNECTION, "IPv%d address: %s (%s)\n", result->ai_family == PF_INET6 ? 6 : 4, addrstr, result->ai_canonname);
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

ThrowablePtr send_http(int sockfd, char *message, size_t total) {
    ssize_t last_sent;  // Last size sent
    ssize_t sent = 0;   // Total size sent

    while (TRUE) {

        // Writes into the socket
        last_sent = write(sockfd, message + (int) sent, total-sent);
        if (last_sent == -1) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "send_http");
        } else if (last_sent == 0) {
            break;
        }

        // Increases number of bytes sent
        sent += last_sent;
    }

    return get_throwable()->create(STATUS_OK, NULL, "send_http");
}

ThrowablePtr send_http_request(int sockfd, HTTPRequestPtr http_request) {
    // Generates the simple request
    char *request_message;
    ThrowablePtr throwable = http_request->make_simple_request(http_request, &request_message);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "send_http_request");
    }

    // Sends request
    throwable = send_http(sockfd, request_message, strlen(request_message));
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "send_http_request");
    }

    return get_throwable()->create(STATUS_OK, NULL, "send_http_request");
}

ThrowablePtr send_http_response_header(int sockfd, HTTPResponsePtr http_response) {
    // Calls more general function receive_http_request
    ThrowablePtr throwable = send_http(sockfd, http_response->response->header, strlen(http_response->response->header));
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "send_http_response_header");
    }

    return get_throwable()->create(STATUS_OK, NULL, "send_http_response_header");
}

ThrowablePtr send_http_chunks(int sockfd, ChunkPtr chunk, int total) {
    ssize_t total_sent = 0; // Total size sent

    while (TRUE) {
        // Gets mutex
        if (pthread_mutex_lock(&chunk->mutex) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "send_http_chunks");
        }

        //get_log()->d(TAG_CONNECTION, "preso lock send");
        // Waits until wrote is FALSE with condition
        while (chunk->wrote == TRUE) { 
            //get_log()->d(TAG_CONNECTION, "aspetto segnale prima send");
            if (pthread_cond_wait(&chunk->condition, &chunk->mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "send_http_chunks");
            }
            //get_log()->d(TAG_CONNECTION, "aspetto segnale dopo send");
        }

        // Sends chunk TODO better higher or lower level?
        ThrowablePtr throwable = send_http(sockfd, chunk->data, chunk->dimen);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            pthread_exit(NULL); // TODO send an error message
        }

        // Increases number of total bytes sent
        total_sent += chunk->dimen;
        
        // Refreshes chunk status
        chunk->dimen = 0;
        chunk->wrote = TRUE;

        // Sends signal to condition
        if (pthread_cond_signal(&chunk->condition) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "send_http_chunks");
        }
        //get_log()->d(TAG_CONNECTION, "inviato segnale send");

        // Releases mutex
        if (pthread_mutex_unlock(&chunk->mutex) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "send_http_chunks");
        }

        //get_log()->d(TAG_CONNECTION, "rilascio lock send");

        // Breaks if all is sent
        if (total == total_sent && total_sent > 0) {
            break;
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "send_http_chunks");
}

ThrowablePtr receive_http(int sockfd, char **message) {

    ssize_t last_received;      // Last size received
    ssize_t total_received = 0; // Total size received
    ssize_t size = 4096;        // Size of message buffer

    // Allocs message buffer
    *message = malloc(size);
    if (message == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "receive_http");
    }

    while (TRUE) {
        // Receives 4096 bytes
        last_received = read(sockfd, *message + total_received, 4096);

        if (last_received == -1) {                                  // There is an error
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http");
        } else if (last_received < 4096 || last_received == 0) {    // EOF reached
            break;
        }

        // Increases number of bytes received
        total_received += last_received;

        // If memory is full realloc
        if (total_received == size) {
            if (realloc(*message, size + 4096) == NULL) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http");
            }
            // Increase size
            size += 4096;
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "receive_http");
}

ThrowablePtr receive_http_header(int sockfd, char **header) {

    ssize_t last_received;      // Last size received
    ssize_t total_received = 0; // Total size received
    ssize_t size = 4096;        // Size of response buffer
    char last_read = ' ';       // Last read characther
    int number = 0;

    // Allocs response buffer
    *header = malloc(size);
    if (header == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "receive_http_header");
    }

    while (TRUE) {
        // If the last bytes read are "\r\n\r\n" the header is complete, so return
        if (number == 4) {
            // Puts EOF
            if (strcpy(*header + total_received, "\0") != *header + total_received) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
            }
            break;
        }

        // Receives 1 byte
        last_received = read(sockfd, &last_read, 1);

        if (last_received == -1) {          // There is an error
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
        } else if (last_received == 0) {    // EOF reached
            break;
        } else {
            // Checks char
            if (last_read == '\n' || last_read == '\r') {
                number++;
            } else {
                number = 0;
            }

            // Copy the response to the header buffer
            if (strcpy(*header + total_received, &last_read) != *header + total_received) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
            }

            // Increases number of bytes received
            total_received += last_received;

            // If memory is full realloc
            if (total_received == size) {
                if (realloc(header, size + 4096) == NULL) {
                    return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_header");
                }
                // Increase size
                size += 4096;
            }
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "receive_http_header");
}

ThrowablePtr receive_http_request(int sockfd, HTTPRequestPtr http_request) {

    // Receives the header
    ThrowablePtr throwable = receive_http_header(sockfd, &http_request->header);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "receive_http_request");
    }

    //get_log()->d(TAG_CONNECTION, "dopo receive receive_http_header");

    // Parses it into the structure
    throwable = http_request->read_headers(http_request, http_request->header, RQST);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "receive_http_request");    
    }

    //get_log()->d(TAG_CONNECTION, "dopo read_headers");

    return get_throwable()->create(STATUS_OK, NULL, "receive_http_request");
}

ThrowablePtr receive_http_response_header(int sockfd, HTTPResponsePtr http_response) {
    // Calls more general function receive_http_request
    ThrowablePtr throwable = receive_http_request(sockfd, http_response->response);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "receive_http_response_header");
    }

    return get_throwable()->create(STATUS_OK, NULL, "receive_http_response_header");
}

ThrowablePtr receive_http_chunks(int sockfd, HTTPResponsePtr http_response, ChunkPtr chunk) {
    ssize_t last_received;      // Last size received
    ssize_t total_received = 0; // Total size read

    while (TRUE) {
        // Gets mutex
        if (pthread_mutex_lock(&chunk->mutex) != 0) {
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
        }

        //get_log()->d(TAG_CONNECTION, "preso lock receive");

        // Waits until wrote is TRUE with condition
        while (chunk->wrote == FALSE) { 
            //get_log()->d(TAG_CONNECTION, "aspetto segnale prima receive");
            if (pthread_cond_wait(&chunk->condition, &chunk->mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
            }
            //get_log()->d(TAG_CONNECTION, "aspetto segnale dopo receive");
        }

        // Calculates the size to read
        int size = (http_response->response->req_content_len - total_received >= MAX_CHUNK_SIZE) ? MAX_CHUNK_SIZE : http_response->response->req_content_len - total_received;

        //get_log()->d(TAG_CONNECTION, "size %d", size);

        // Reads from the socket and it puts the response into the chunk
        last_received = read(sockfd, chunk->data, size); 
        if (last_received == -1) {          // There is an error
            return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
        } else if (last_received == 0) {    // EOF reached
            break;
        } else {

            // Increases number of total bytes read
            total_received += last_received;
            
            // Refreshes chunk status
            chunk->dimen = last_received;
            chunk->wrote = FALSE;

            // Sends signal to condition
            if (pthread_cond_signal(&chunk->condition) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
            }
            //get_log()->d(TAG_CONNECTION, "inviato segnale receive");

            // Releases mutex
            if (pthread_mutex_unlock(&chunk->mutex) != 0) {
                return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "receive_http_chunks");
            }
            //get_log()->d(TAG_CONNECTION, "rilascio lock receive");

            // Breaks if all is read
            if (total_received == http_response->response->req_content_len) {
                break;
            }
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "receive_http_chunks");
}