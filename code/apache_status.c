//
//============================================================================
// Name             : apache_status.c
// Author           : Claudio Pastorini
// Version          : 0.3
// Data Created     : 09/05/2015
// Last modified    : 30/05/2015
// Description      : This file contains all the stuffs useful in order to
//                    reads the status of an Apache server.
//                    For other info read:
//                      - http://httpd.apache.org/docs/2.4/mod/mod_status.html
// ===========================================================================
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "helper.h"
#include "connection.h"
#include "log.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : parse_apache_status
 * Description: This function parse the HTML page from ApacheServerStatus and
 *              put all the data into that.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 * ---------------------------------------------------------------------------
 */
Throwable *parse_apache_status(ApacheServerStatus *self) {
    // TODO: improve with @alessio
    char *to_parse = strdup(self->status_page);  // The string to parse
    char *delim = ":";                      // First delimiter
    char *sub_delim = "\n";                 // Second delimiter
    char *str1, *str2, *token, *sub_token;  // Useful string
    char *save_ptr1, *save_ptr2;            // Useful pointer
    int j, i;

    for (j = 1, i = 0, str1 = to_parse; ; j++, str1 = NULL) {
        token = strtok_r(str1, delim, &save_ptr1);
        if (token == NULL) {
            break;
        }

        for (str2 = token; ; str2 = NULL) {
            sub_token = strtok_r(str2, sub_delim, &save_ptr2);
            if (sub_token == NULL) {
                break;
            }
            i++;
            if (i % 2 == 0) {
                switch (i) {
                    case 2: {  // Total Accesses
                        int total_accesses = str_to_int(sub_token);

                        if (total_accesses == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->total_accesses = total_accesses;
                        break;
                    }
                    case 4: {  // Total kBytes
                        int total_kBytes = str_to_int(sub_token);

                        if (total_kBytes == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->total_kBytes = total_kBytes;
                        break;
                    }
                    case 6: {  // CPULoad
                        float cpu_load = str_to_float(sub_token);

                        if (cpu_load == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->cpu_load = cpu_load;
                        break;
                    }
                    case 8: {  // Uptime
                        int uptime = str_to_int(sub_token);

                        if (uptime == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->uptime = uptime;
                        break;
                    }
                    case 10: {  // ReqPerSec
                        float req_per_sec = str_to_float(sub_token);

                        if (req_per_sec == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->req_per_sec = req_per_sec;
                        break;
                    }
                    case 12: {  // BytesPerSec
                        float bytes_per_sec = str_to_float(sub_token);

                        if (bytes_per_sec == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->bytes_per_sec = bytes_per_sec;
                        break;
                    }
                    case 14: {  // BytesPerReq
                        float bytes_per_req = str_to_float(sub_token);

                        if (bytes_per_req == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->bytes_per_req = bytes_per_req;
                        break;
                    }
                    case 16: {  // BusyWorkers
                        int busy_workers = str_to_int(sub_token);

                        if (busy_workers == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->busy_workers = busy_workers;
                        break;
                    }
                    case 18: {  // IdleWorkers
                        int idle_workers = str_to_int(sub_token);

                        if (idle_workers == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Unable to parse", "parse_apache_status");
                        self->idle_workers = idle_workers;
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    // Free memory used for to_parse
    free(to_parse);

    return (*get_throwable()).create(STATUS_OK, "", "parse_apache_status");
}

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_apache_status
 * Description: This function free all the memory bind with ApacheServerStatus.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_apache_status(void *self) {
    free(((ApacheServerStatus *) self)->url);
    free(((ApacheServerStatus *) self)->status_page);
    free(self);
}

/*
 * ---------------------------------------------------------------------------
 * Function   : retrieve_apache_status
 * Description: This function retrieve the HTML page from URL of the
 *              ApacheServerStatus and put all the data into that.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 * ---------------------------------------------------------------------------
 */
Throwable *retrieve_apache_status(ApacheServerStatus *self) {
    // TODO improve with @andrea creating a helper function to retrieve HTTP page
    int sockfd;
    if (create_client_socket(TCP, "5.196.1.149", 80, &sockfd) == STATUS_ERROR) return (*get_throwable()).create(STATUS_ERROR, "Boh", "retrieve_apache_status");

    fprintf(stdout, "Connected to server at URL: %s\n", self->url);

    int bytes, sent, received, total;
    char message[1024], response[4096];

    // Fill in the parameters TODO use the new http_request struct
    sprintf(message, "GET /server-status?auto HTTP/1.1\nHost: www.laziobus.it\n\n");

    (*get_log()).d(TAG_APACHE_STATUS, message);

    // Send the request
    total = (int) strlen(message);
    sent = 0;
    do {
        bytes = (int) write(sockfd, message+sent, (size_t) (total-sent));
        if (bytes < 0)
            fprintf(stderr, "ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    // Receive the response
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do {
        bytes = (int) read(sockfd,response-received, (size_t) (total-received));
        if (bytes < 0)
            fprintf(stderr, "ERROR reading response from socket");
        if (bytes == 0)
            break;
        received += bytes;
    } while (received < total);

    if (received == total)
        fprintf(stderr, "ERROR storing complete response from socket");

    // Close the socket
    close(sockfd);

    (*get_log()).d(TAG_APACHE_STATUS, response);

    //TODO read and manage header and put in status page only the content of the http response
    self->status_page = strdup("Total Accesses: 143\nTotal kBytes: 340\nCPULoad: .125764\nUptime: 1145\nReqPerSec: .124891\nBytesPerSec: 304.07\nBytesPerReq: 2434.69\nBusyWorkers: 1\nIdleWorkers: 7\nScoreboard: _____W__..............................................................................................................................................");

    return (*get_throwable()).create(STATUS_OK, "", "retrieve_apache_status");
}

/*
 * ---------------------------------------------------------------------------
 * Function   : set_url_apache_status
 * Description: This function set the URL of the ApacheServerStatus.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *   url    : The URL of the ApacheServerStatus.
 *
 * Return     :
 * ---------------------------------------------------------------------------
 */
Throwable *set_url_apache_status(ApacheServerStatus *self, char *url) {
    errno = 0;
    char *new_url = strdup(url);


/* // TODO use something similar in order to get entire error
    char *s;
    size_t size;

    size = 1024;
    s = malloc(size);
    if (s == NULL)
        return NULL;

    while (strerror_r(n, s, size) == -1 && errno == ERANGE) {
        size *= 2;
        s = realloc(s, size);
        if (s == NULL)
            return NULL;
    }
*/


    if (new_url == NULL) {
        //char *message =
        return (*get_throwable()).create(STATUS_OK, "BOH", "set_url_apache_status");
    }
    self->url = new_url;

    return (*get_throwable()).create(STATUS_OK, "", "set_url_apache_status");

}

/*
 * ---------------------------------------------------------------------------
 * Function   : to_string_apache_status
 * Description: This function return a char pointer with a human form of the
 *              ApacheServerStatus.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *
 * Return     : The pointer of the string.
 * ---------------------------------------------------------------------------
 */
char *to_string_apache_status(ApacheServerStatus *self) {  // TODO doubt about this memory management
    char *string = malloc(sizeof(char) * 64000);  // TODO i do not know how many space
    snprintf(string, 64000,
             "Status of Apache server at URL: %s\n\n"
             "Total Accesses: %d\n"
             "Total kBytes: %d\n"
             "CPULoad: %f\n"
             "Uptime: %d\n"
             "ReqPerSec: %f\n"
             "BytesPerSec: %f\n"
             "BytesPerReq: %f\n"
             "BusyWorkers: %d\n"
             "IdleWorkers: %d",
             self->url,
             self->total_accesses,
             self->total_kBytes,
             self->cpu_load,
             self->uptime,
             self->req_per_sec,
             self->bytes_per_sec,
             self->bytes_per_req,
             self->busy_workers,
             self->idle_workers);
    return string;
}

ApacheServerStatus *new_apache_server_status() {

    ApacheServerStatus *apache_server_status = malloc(sizeof(ApacheServerStatus));
    if (apache_server_status == NULL) {
        fprintf(stderr, "Memory allocation error in new_apache_server_status.\n");
        exit(EXIT_FAILURE);
    }
    // Set self linking
    apache_server_status->self = apache_server_status;

    // Set "methods"
    apache_server_status->retrieve = retrieve_apache_status;
    apache_server_status->parse = parse_apache_status;
    apache_server_status->destroy = destroy_apache_status;
    apache_server_status->set_url = set_url_apache_status;
    apache_server_status->to_string = to_string_apache_status;

    return apache_server_status;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {

    int n = 1; // Number of arguments
    if (argc != n + 1 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage %s <URL>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Log Log = (*get_log());

    // Initialize server_status
    ApacheServerStatus server_status = *new_apache_server_status();

    // Set url of apache server
    server_status.set_url(server_status.self, argv[1]);

    // Retrieve status
    Throwable *retrieve_throwable = server_status.retrieve(server_status.self);
    if ((*retrieve_throwable).status == STATUS_ERROR) {
        Log.print_throwable(retrieve_throwable);
        exit(EXIT_FAILURE);
    }

    // Parse
    Throwable *parse_throwable = server_status.parse(server_status.self);
    if ((*parse_throwable).status == STATUS_ERROR) {
        Log.print_throwable(retrieve_throwable);
        exit(EXIT_FAILURE);
    }

    Log.i(TAG_APACHE_STATUS, server_status.to_string(server_status.self));

    // Destroy the object
    server_status.destroy(server_status.self);

    return EXIT_SUCCESS;
}