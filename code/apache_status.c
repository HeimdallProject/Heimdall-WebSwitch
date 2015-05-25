//
//============================================================================
// Name       : apache_status.c
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: This file contains all the stuffs useful in order to
//              reads the status of an Apache server.
//              For other info read:
//               - http://httpd.apache.org/docs/2.4/mod/mod_status.html
// ===========================================================================
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "helper.h"
#include "connection.h"
#include "apache_status.h"

int retrieve_apache_status(char *url, char **status_page) {

    int sockfd;
    sockfd = create_client_socket(TCP, "5.196.1.149", 80);

    fprintf(stdout, "Connected to server at URL: %s\n", url);

    int bytes, sent, received, total;
    char message[1024],response[4096];

    /* fill in the parameters */
    sprintf(message, "GET /server-status?auto HTTP/1.1\nHost: www.laziobus.it\n\n");
    printf("Request:\n%s\n",message);

    /* send the request */
    total = (int) strlen(message);
    sent = 0;
    do {
        bytes = (int) write(sockfd,message+sent, (size_t) (total-sent));
        if (bytes < 0)
            fprintf(stderr, "ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = (int) read(sockfd,response-received, (size_t) (total-received));
        if (bytes < 0)
            fprintf(stderr, "ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if (received == total)
        fprintf(stderr, "ERROR storing complete response from socket");

    /* close the socket */
    close(sockfd);

    /* process response */
    printf("Response:\n%s\n", response);

    *status_page = "Total Accesses: 143\nTotal kBytes: 340\nCPULoad: .125764\nUptime: 1145\nReqPerSec: .124891\nBytesPerSec: 304.07\nBytesPerReq: 2434.69\nBusyWorkers: 1\nIdleWorkers: 7\nScoreboard: _____W__..............................................................................................................................................";

    return STATUS_OK;
}

int parse_apache_status(char **status_page, struct apache_server_status* server_status) {

    char *to_parse = strdup(*status_page);  // The string to parse
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

                        if (total_accesses != STATUS_ERROR) {
                            server_status->total_accesses = total_accesses;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 4: {  // Total kBytes
                        int total_kBytes = str_to_int(sub_token);

                        if (total_kBytes != STATUS_ERROR) {
                            server_status->total_kBytes = total_kBytes;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 6: {  // CPULoad
                        float cpu_load = str_to_float(sub_token);

                        if (cpu_load != STATUS_ERROR) {
                            server_status->cpu_load = cpu_load;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 8: {  // Uptime
                        int uptime = str_to_int(sub_token);

                        if (uptime != STATUS_ERROR) {
                            server_status->uptime = uptime;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 10: {  // ReqPerSec
                        float req_per_sec = str_to_float(sub_token);

                        if (req_per_sec != STATUS_ERROR) {
                            server_status->req_per_sec = req_per_sec;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return -1;
                        }
                        break;
                    }
                    case 12: {  // BytesPerSec
                        float bytes_per_sec = str_to_float(sub_token);

                        if (bytes_per_sec != STATUS_ERROR) {
                            server_status->bytes_per_sec = bytes_per_sec;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 14: {  // BytesPerReq
                        float bytes_per_req = str_to_float(sub_token);

                        if (bytes_per_req != STATUS_ERROR) {
                            server_status->bytes_per_req = bytes_per_req;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 16: {  // BusyWorkers
                        int busy_workers = str_to_int(sub_token);

                        if (busy_workers != STATUS_ERROR) {
                            server_status->busy_workers = busy_workers;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
                        break;
                    }
                    case 18: {  // IdleWorkers
                        int idle_workers = str_to_int(sub_token);

                        if (idle_workers != STATUS_ERROR) {
                            server_status->idle_workers = idle_workers;
                        } else {
                            fprintf(stderr, "Unable to parse\n");
                            return STATUS_ERROR;
                        }
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

    return STATUS_OK;
}

void print_apache_status(struct apache_server_status* server_status) {
    fprintf(stdout,
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
            server_status->url,
            server_status->total_accesses,
            server_status->total_kBytes,
            server_status->cpu_load,
            server_status->uptime,
            server_status->req_per_sec,
            server_status->bytes_per_sec,
            server_status->bytes_per_req,
            server_status->busy_workers,
            server_status->idle_workers);
}

int main(int argc, char *argv[]) {

    int n = 1; // Number of arguments
    if (argc != n + 1 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage %s <URL>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create struct and alloc memory for apache_server_status
    struct apache_server_status *server_status;
    server_status = malloc(1 * sizeof(struct apache_server_status));
    if (server_status == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }

    // Set url of apache server
    server_status->url = argv[1];

    // Alloc memory for status_page
    char **status_page;
    status_page = malloc(5000 * sizeof(char *));  // TODO: how many char?
    if (status_page == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }

    // Retrieve page from URL and put into status_page
    int status_retrieve;
    status_retrieve = retrieve_apache_status(server_status->url, status_page);

    int status_parse;
    if (status_retrieve == STATUS_OK) {                                  // If retrieved
        status_parse = parse_apache_status(status_page, server_status);  // Parse status_page and put into server_status
    } else {
        fprintf(stderr, "Error in retriving status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for status_page
    free(status_page);

    if (status_parse == STATUS_OK) {         // If parsed
        print_apache_status(server_status);  // Print server_status
    } else {
        fprintf(stderr, "Error in parsing status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for server_status
    free(server_status);

    return EXIT_SUCCESS;
}