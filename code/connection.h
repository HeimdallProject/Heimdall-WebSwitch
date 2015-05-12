//
//============================================================================
// Name       : connection.h
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: Header file for our client/server project with simple socket's
//              functions.
// ============================================================================
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>

#define TCP 0
#define UDP 1


//TODO create struct for HTTP request
//TODO create struct for HTTP response
//TODO create wrapper for *gethostbyname
/*
 *
 */
int create_socket(int type) {

    int sockfd;
    if (type == 0) {  // TCP
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Error in socket()");
            exit(EXIT_FAILURE);
        }
    } else if (type == 1) {  // UDP
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Error in socket()");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Usage create_socket_and_bind(<type>), <type> = 0 for TCP <type> = 1 for UDP\n");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/*
 *
 */
int create_server_socket(int type, int port) {

    int sockfd;
    sockfd = create_socket(type);

    struct sockaddr_in addr;

    memset((void *) &addr, 0, sizeof(addr));   // Set all memory to 0
    addr.sin_family = AF_INET;                 // Set IPV4 family
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Waiting a connection on all server's IP addresses
    addr.sin_port = htons((uint16_t) port);    // Waiting a connection on PORT

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Error in bind()");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/*
 *
 */
int create_client_socket(int type, char *ip, int port) {

    int sockfd;
    sockfd = create_socket(type);

    struct sockaddr_in addr;

    memset((void *) &addr, 0, sizeof(addr));  // Set all memory to 0
    addr.sin_family = AF_INET;                // Set IPV4 family
    addr.sin_port = htons((uint16_t) port);	  // Set server connection on specified PORT

    if (inet_pton(AF_INET, ip, &addr.sin_addr) == -1) {
        perror("Error in inet_pthon()");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Error in connect()");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/*
 *
 */
void listen_to(int sockfd, int backlog) {

    if (listen(sockfd, backlog) == -1) {
        perror("Error in listen()");
        exit(EXIT_FAILURE);
    }
}

int create_connection(int sockfd) {

    int connection;
    if ((connection = accept(sockfd, (struct sockaddr *)NULL, NULL)) == -1) {
        perror("Error in accept()");
        exit(EXIT_FAILURE);
    }

    return connection;
}

/*
 *
 */
void close_connection(int connection) {

    if (close(connection) == -1) {
        perror("Error in close()");
        exit(EXIT_FAILURE);
    }
}