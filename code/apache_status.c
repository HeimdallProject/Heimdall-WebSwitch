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
#include "helper.h"

/*
 * ---------------------------------------------------------------------------
 * Structure:  apache_server_status
 * Description: This struct collect all data about the state of a Apace server.
 *
 * Data:
 *   url           : The URL of the server status page.
 *   total_accesses: A total number of accesses served.
 *   total_kBytes  : A total number of byte count served.
 *   cpu_load      : The current percentage CPU used by each worker and in total
 *                   by all workers combined
 *   uptime        : The time the server was started/restarted and the time it
 *                   has been running for.
 *   req_per_sec   : Averages giving the number of requests per second, the
 *                   number of bytes served per second and the average number
 *                   of bytes per request.
 *   bytes_per_sec : The number of bytes served per second.
 *   bytes_per_req : The average number of bytes per request.
 *   busy_workers  : The number of worker serving requests.
 *   idle_workers  : The number of idle worker.
 */
struct apache_server_status {
    char *url;
    int total_accesses;
    int total_kBytes;
    float cpu_load;
    int uptime;
    float req_per_sec;
    float bytes_per_sec;
    float bytes_per_req;
    int busy_workers;
    int idle_workers;
};

/*
 * ---------------------------------------------------------------------------
 * Function:     retrieve_apache_status
 * Description:  This function retrieve the HTML page of the given URL. It
 *               save the page into status_page.
 *
 * Param:
 *   url        : The URL of the page to retrieve.
 *   status_page: The pointer in which save the page retrieved.
 *
 * Return:       STATUS_OK in case of success, STATUS_ERROR otherwise.
 * ---------------------------------------------------------------------------
 */
int retrieve_apache_status(char *url, char **status_page) {

    *status_page = "Total Accesses: 143\nTotal kBytes: 340\nCPULoad: .125764\nUptime: 1145\nReqPerSec: .124891\nBytesPerSec: 304.07\nBytesPerReq: 2434.69\nBusyWorkers: 1\nIdleWorkers: 7\nScoreboard: _____W__..............................................................................................................................................";

    return STATUS_OK;
}

/*
 * ---------------------------------------------------------------------------
 * Function:     parse_apache_status
 * Description:  This function parse the HTML page given in order to
 *               get all the info about Apache server.
 *
 * Param:
 *   status_page  : The pointer in which is saved the page to parse.
 *   server_status: The struct in which save the data parsed.
 *
 * Return:       STATUS_OK in case of success, STATUS_ERROR otherwise.
 * ---------------------------------------------------------------------------
 */
int parse_apache_status(char **status_page, struct apache_server_status* server_status) {

    char *to_parse = strdup(*status_page);  // The string to parse
    char *delim = ":";  // First delimiter
    char *sub_delim = "\n";  // Second delimiter
    char *str1, *str2, *token, *sub_token;  // Useful string
    char *save_ptr1, *save_ptr2;  // Useful pointer
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

/*
 * ---------------------------------------------------------------------------
 * Function:     print_apache_status
 * Description:  This function print the status stored into
 *               apache_server_status struct.
 *
 * Param:
 *   server_status : The pointer of the struct to print.
 *
 * ---------------------------------------------------------------------------
 */
void print_apache_status(struct apache_server_status* server_status) {
    fprintf(stdout, "Status of Apache server at URL: %s\n\nTotal Accesses: %d\nTotal kBytes: %d\nCPULoad: %f\nUptime: "
            "%d\nReqPerSec: %f\nBytesPerSec: %f\nBytesPerReq: %f\nBusyWorkers: %d\nIdleWorkers: %d", server_status->url,
            server_status->total_accesses, server_status->total_kBytes, server_status->cpu_load, server_status->uptime,
            server_status->req_per_sec, server_status->bytes_per_sec, server_status->bytes_per_req,
            server_status->busy_workers, server_status->idle_workers);
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
    if (status_retrieve == STATUS_OK) {  // If retrieved
        status_parse = parse_apache_status(status_page, server_status);  // Parse status_page and put into server_status
    } else {
        fprintf(stderr, "Error in retriving status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for status_page
    free(status_page);

    if (status_parse == STATUS_OK) {  // If parsed
        print_apache_status(server_status);  // Print server_status
    } else {
        fprintf(stderr, "Error in parsing status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for server_status
    free(server_status);

    return EXIT_SUCCESS;
}