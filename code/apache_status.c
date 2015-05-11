
// http://httpd.apache.org/docs/2.4/mod/mod_status.html

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*
 * Structure:  apache_server_status
 * --------------------
 * Description
 *
 *
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
 * Function:  retrieve_apache_status
 * --------------------
 * Description
 *
 *  url:
 *  status_page:
 *
 *  returns:
 */
int retrieve_apache_status(char *url, char **status_page) {

    *status_page = "Total Accesses: 143\nTotal kBytes: 340\nCPULoad: .125764\nUptime: 1145\nReqPerSec: .124891\nBytesPerSec: 304.07\nBytesPerReq: 2434.69\nBusyWorkers: 1\nIdleWorkers: 7\nScoreboard: _____W__..............................................................................................................................................";

    return 1;
}

int str_to_int(char *string) {

    char *pointer;
    int value;

    errno = 0;
    value = (int) strtol(string, &pointer, 0);
    if (errno != 0 || *pointer != '\0') {
        fprintf(stderr, "Invalid number");
        return -1;
    }

    return value;
}

float str_to_float(char *string) {

    char *pointer;
    float value;

    errno = 0;
    value =  strtof(string, &pointer);
    if (errno != 0 || *pointer != '\0') {
        fprintf(stderr, "Invalid number");
        return -1;
    }

    return value;
}

/*
 * Function:  parse_apache_status
 * --------------------
 * Description
 *
 *  status_page:
 *  server_status:
 *
 *  returns:
 */
int parse_apache_status(char **status_page, struct apache_server_status* server_status) {


    char *to_parse = strdup(*status_page);
    char *delim = ":";
    char *sub_delim = "\n";
    char *str1, *str2, *token, *sub_token;
    char *save_ptr1, *save_ptr2;
    int j, i;

    for (j = 1, i = 0, str1 = to_parse; ; j++, str1 = NULL) {
        token = strtok_r(str1, delim, &save_ptr1);
        if (token == NULL) {
            break;
        }
        //printf("%d: %s\n", j, token);

        for (str2 = token; ; str2 = NULL) {
            sub_token = strtok_r(str2, sub_delim, &save_ptr2);
            if (sub_token == NULL) {
                break;
            }
            i++;
            if (i % 2 == 0) {
                //printf("%d: %s\n", i, sub_token);
                switch (i) {
                    case 2: {
                        int total_accesses = str_to_int(sub_token);

                        if (total_accesses != -1) {
                            server_status->total_accesses = total_accesses;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 4: {
                        int total_kBytes = str_to_int(sub_token);

                        if (total_kBytes != -1) {
                            server_status->total_kBytes = total_kBytes;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 6: {
                        float cpu_load = str_to_float(sub_token);

                        if (cpu_load != -1) {
                            server_status->cpu_load = cpu_load;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 8: {
                        int uptime = str_to_int(sub_token);

                        if (uptime != -1) {
                            server_status->uptime = uptime;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 10: {
                        float req_per_sec = str_to_float(sub_token);

                        if (req_per_sec != -1) {
                            server_status->req_per_sec = req_per_sec;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 12: {
                        float bytes_per_sec = str_to_float(sub_token);

                        if (bytes_per_sec != -1) {
                            server_status->bytes_per_sec = bytes_per_sec;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 14: {
                        float bytes_per_req = str_to_float(sub_token);

                        if (bytes_per_req != -1) {
                            server_status->bytes_per_req = bytes_per_req;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 16: {
                        int busy_workers = str_to_int(sub_token);

                        if (busy_workers != -1) {
                            server_status->busy_workers = busy_workers;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    case 18: {
                        int idle_workers = str_to_int(sub_token);

                        if (idle_workers != -1) {
                            server_status->idle_workers = idle_workers;
                        } else {
                            fprintf(stderr, "Unable to parse");
                            return -1;
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return 1;
}

/*
 * Function:  print_apache_status
 * --------------------
 * Description
 *
 *  server_status:
 *
 *  returns:
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
    if (status_retrieve) {  // If retrieved
        status_parse = parse_apache_status(status_page, server_status);  // Parse status_page and put into server_status
    } else {
        fprintf(stderr, "Error in retriving status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for status_page
    free(status_page);

    if (status_parse) {  // If parsed
        print_apache_status(server_status);  // Print server_status
    } else {
        fprintf(stderr, "Error in parsing status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for server_status
    free(server_status);

    return EXIT_SUCCESS;
}