
// http://httpd.apache.org/docs/2.4/mod/mod_status.html

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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


    char *string = *status_page;

    int counter = 0;

    int i = 0;
    while(*(string + i * sizeof(char)) != '\0' && counter < 9) {

        if (*(string + i * sizeof(char)) == ':') {

            int j = i + 2;
            while(*(string + j * sizeof(char)) != '\n') {
                printf("%c", *(string + j * sizeof(char)));
                j++;
            }

            counter++;
            printf("\n");
        }

        i++;
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
        //print_apache_status(server_status);  // Print server_status
    } else {
        fprintf(stderr, "Error in parsing status page, please retry.\n");
        return EXIT_FAILURE;
    }

    // Free memory used for server_status
    free(server_status);

    return EXIT_SUCCESS;
}