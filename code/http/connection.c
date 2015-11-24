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

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
/*
int main() {

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

    //log->i(TAG_CONNECTION, http_response->http_response_body); //TODO non funziona...

    // Destroy the object
    http_request->destroy(http_request);

    return EXIT_SUCCESS;
}*/
