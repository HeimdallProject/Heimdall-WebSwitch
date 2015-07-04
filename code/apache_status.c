//
//============================================================================
// Name             : apache_status.c
// Author           : Claudio Pastorini
// Version          : 0.3
// Data Created     : 09/05/2015
// Last modified    : 01/06/2015
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
#include "apache_status.h"
#include "log.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : parse_apache_status
 * Description: This function parse the HTML page from ApacheServerStatus and
 *              put all the data into that.
 *
 * Param      :
 *   self       : The pointer to the ApacheServerStatus.
 *
 * Return     : A Throwable.
 * ---------------------------------------------------------------------------
 */
Throwable *parse_apache_status(ApacheServerStatus *self) {
    // TODO: improve with @alessio
    char *to_parse = strdup(self->status_page); // The string to parse
    if (errno != 0 || to_parse == NULL) return (*get_throwable()).create(STATUS_OK, get_error_by_errno(errno), "parse_apache_status");
    char *delim = ":";                          // First delimiter
    char *sub_delim = "\n";                     // Second delimiter
    char *str1, *str2, *token, *sub_token;      // Useful strings
    char *save_ptr1, *save_ptr2;                // Useful pointers
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
                    case 2: {  // total_accesses
                        Throwable total_access_throwable = *str_to_int(sub_token, &(self->total_accesses));
                        if (total_access_throwable.is_an_error(total_access_throwable.self)) return total_access_throwable.thrown(total_access_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 4: {  // total_kBytes
                        Throwable total_kBytes_throwable = *str_to_int(sub_token, &(self->total_kBytes));
                        if (total_kBytes_throwable.is_an_error(total_kBytes_throwable.self)) return total_kBytes_throwable.thrown(total_kBytes_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 6: {  // cpu_load
                        Throwable cpu_load_throwable = *str_to_float(sub_token, &(self->cpu_load));
                        if (cpu_load_throwable.is_an_error(cpu_load_throwable.self)) return cpu_load_throwable.thrown(cpu_load_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 8: {  // uptime
                        Throwable uptime_throwable = *str_to_int(sub_token, &(self->uptime));
                        if (uptime_throwable.is_an_error(uptime_throwable.self)) return uptime_throwable.thrown(uptime_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 10: {  // req_per_sec
                        Throwable req_per_sec_throwable = *str_to_float(sub_token, &(self->req_per_sec));
                        if (req_per_sec_throwable.is_an_error(req_per_sec_throwable.self)) return req_per_sec_throwable.thrown(req_per_sec_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 12: {  // bytes_per_sec
                        Throwable bytes_per_sec_throwable = *str_to_float(sub_token, &(self->bytes_per_sec));
                        if (bytes_per_sec_throwable.is_an_error(bytes_per_sec_throwable.self)) return bytes_per_sec_throwable.thrown(bytes_per_sec_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 14: {  // bytes_per_req
                        Throwable bytes_per_req_throwable = *str_to_float(sub_token, &(self->bytes_per_req));
                        if (bytes_per_req_throwable.is_an_error(bytes_per_req_throwable.self)) return bytes_per_req_throwable.thrown(bytes_per_req_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 16: {  // busy_workers
                        Throwable busy_workers_throwable = *str_to_int(sub_token, &(self->busy_workers));
                        if (busy_workers_throwable.is_an_error(busy_workers_throwable.self)) return busy_workers_throwable.thrown(busy_workers_throwable.self, "parse_apache_status");
                        break;
                    }
                    case 18: {  // idle_workers
                        Throwable idle_workers_throwable = *str_to_int(sub_token, &(self->idle_workers));
                        if (idle_workers_throwable.is_an_error(idle_workers_throwable.self)) return idle_workers_throwable.thrown(idle_workers_throwable.self, "parse_apache_status");
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

    return (*get_throwable()).create(STATUS_OK, NULL, "parse_apache_status");
}

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_apache_status
 * Description: This function free all the memory bound with ApacheServerStatus.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void destroy_apache_status(void *self) {
    free(((ApacheServerStatus *) self)->url);           // Free space for URL
    free(((ApacheServerStatus *) self)->status_page);   // Free space for status page
    free(((ApacheServerStatus *) self)->string);        // Free space for to_string
    free(self);                                         // Free all the struct
}

/*
 * ---------------------------------------------------------------------------
 * Function   : retrieve_apache_status
 * Description: This function retrieve the HTML page from URL of the
 *              ApacheServerStatus and put the content in status_page.
 *
 * Param      :
 *   self   : The pointer to the ApacheServerStatus.
 *
 * Return     : A Throwable.
 * ---------------------------------------------------------------------------
 */
Throwable *retrieve_apache_status(ApacheServerStatus *self) {
    // TODO improve with @andrea creating a helper function to retrieve HTTP page
    /*int sockfd;
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

    (*get_log()).d(TAG_APACHE_STATUS, response);*/

    //TODO read and manage header and put in status page only the content of the http response
    self->status_page = strdup("Total Accesses: 143\nTotal kBytes: 340\nCPUaLoad: .125764\nUptime: 1145\nReqPerSec: .124891\nBytesPerSec: 304.07\nBytesPerReq: 2434.69\nBusyWorkers: 1\nIdleWorkers: 7\nScoreboard: _____W__..............................................................................................................................................");

    return (*get_throwable()).create(STATUS_OK, NULL, "retrieve_apache_status");
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
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
Throwable *set_url_apache_status(ApacheServerStatus *self, char *url) {
    errno = 0;
    char *new_url = strdup(url);
    if (errno != 0 || new_url == NULL) return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "set_url_apache_status");
    // Set url
    self->url = new_url;

    return (*get_throwable()).create(STATUS_OK, NULL, "set_url_apache_status");
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
char *to_string_apache_status(ApacheServerStatus *self) {
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

    self->string = string;  //  Save pointer in the struct in order to free after
    return string;
}

ApacheServerStatus *new_apache_server_status() {

    ApacheServerStatus *apache_server_status = malloc(sizeof(ApacheServerStatus));
    if (apache_server_status == NULL) {
        (*get_log()).e(TAG_APACHE_STATUS, "Memory allocation error in new_apache_server_status.\n");
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
    Throwable retrieve_throwable = *server_status.retrieve(server_status.self);
    if (retrieve_throwable.is_an_error(retrieve_throwable.self)) {
        Log.t(&retrieve_throwable);
        exit(EXIT_FAILURE);     // Or throw again
    }

    // Parse
    Throwable parse_throwable = *server_status.parse(server_status.self);
    if (parse_throwable.is_an_error(parse_throwable.self)) {
        Log.t(&parse_throwable);
        exit(EXIT_FAILURE);     // Or throw again
    }

    Log.i(TAG_APACHE_STATUS, server_status.to_string(server_status.self));

    // Destroy the object
    server_status.destroy(server_status.self);

    return EXIT_SUCCESS;
}