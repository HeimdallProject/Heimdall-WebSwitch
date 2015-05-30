//
//============================================================================
// Name       : apache_status.c
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: This header file contains all the stuffs useful in order to
//              reads the status of an Apache server.
//              For other info read:
//               - http://httpd.apache.org/docs/2.4/mod/mod_status.html
// ===========================================================================
//

/*
 * ---------------------------------------------------------------------------
 * Structure  : apache_server_status
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
 *
 * Functions:
 *  parse_apache_status     : Pointer to parse apache status function.
 *  destroy                 : Pointer to destroy function.
 */
typedef struct apache_server_status {
    struct apache_server_status *self;
    char *url;
    char *status_page;
    int total_accesses;
    int total_kBytes;
    float cpu_load;
    int uptime;
    float req_per_sec;
    float bytes_per_sec;
    float bytes_per_req;
    int busy_workers;
    int idle_workers;
    int (*retrieve)(struct apache_server_status *self);
    int (*parse)(struct apache_server_status *self);
    void (*destroy)(void *self);
    void (*set_url)(struct apache_server_status *self, char *url);
} ApacheServerStatus;

/*
 * ---------------------------------------------------------------------------
 * Function   : retrieve_apache_status
 * Description: This function retrieve the HTML page of the given URL. It
 *              save the page into status_page.
 *
 * Param      :
 *   url        : The URL of the page to retrieve.
 *   status_page: The pointer in which save the page retrieved.
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 * ---------------------------------------------------------------------------
 */
int retrieve_apache_status(ApacheServerStatus *self);

/*
 * ---------------------------------------------------------------------------
 * Function   : parse_apache_status
 * Description: This function parse the HTML page given in order to
 *              get all the info about Apache server.
 *
 * Param      :
 *   status_page  : The pointer in which is saved the page to parse.
 *   server_status: The struct in which save the data parsed.
 *
 * Return     : STATUS_OK in case of success, STATUS_ERROR otherwise.
 * ---------------------------------------------------------------------------
 */
int parse_apache_status(ApacheServerStatus *self);