//
//============================================================================
// Name       : connection.h
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: Header file for our client/server project with simple socket's
//              functions.
// ============================================================================
//

#define TCP 0
#define UDP 1


//TODO create struct for HTTP request
//TODO create struct for HTTP response
//TODO create wrapper for *gethostbyname
/*
 *
 */
int create_socket(int type);

/*
 *
 */
int create_server_socket(int type, int port);

/*
 *
 */
int create_client_socket(int type, char *ip, int port);

/*
 *
 */
void listen_to(int sockfd, int backlog);

/*
 *
 */
int create_connection(int sockfd);

/*
 *
 */
void close_connection(int connection);