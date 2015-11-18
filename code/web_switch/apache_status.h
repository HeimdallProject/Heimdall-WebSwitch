//
//============================================================================
// Name             : apache_status.c
// Author           : Claudio Pastorini
// Version          : 0.3
// Data Created     : 09/05/2015
// Last modified    : 01/06/2015
// Description      : This header file contains all the stuffs useful in order to
//                    reads the status of an Apache server.
//                    For other info read:
//                      - http://httpd.apache.org/docs/2.4/mod/mod_status.html
// ===========================================================================
//
#ifndef APACHE_STATUS_H
#define APACHE_STATUS_H

#define TAG_APACHE_STATUS "APACHE_STATUS"  // Macro for log message

/*
 * ---------------------------------------------------------------------------
 * Structure  : apache_server_status
 * Description: This struct collect all data about the state of a Apace server.
 *
 * Data:
 *  self            : Pointer to itself.
 *  url             : The URL of the server status page.
 *  status_page     : The complete status page.
 *  string          : The summary of struct.
 *  total_accesses  : A total number of accesses served.
 *  total_kBytes    : A total number of byte count served.
 *  cpu_load        : The current percentage CPU used by each worker and in total
 *                    by all workers combined
 *  uptime          : The time the server was started/restarted and the time it
 *                    has been running for.
 *  req_per_sec     : Averages giving the number of requests per second, the
 *                    number of bytes served per second and the average number
 *                    of bytes per request.
 *  bytes_per_sec   : The number of bytes served per second.
 *  bytes_per_req   : The average number of bytes per request.
 *  busy_workers    : The number of worker serving requests.
 *  idle_workers    : The number of idle worker.
 *
 * Functions:
 *  retrieve   : Pointer to retrieve_apache_status function.
 *  parse      : Pointer to parse_apache_status function.
 *  set_url    : Pointer to set_url function.
 *  to_string  : Pointer to to_string function.
 *  destroy    : Pointer to destroy function.
 */
typedef struct apache_server_status {
    struct apache_server_status *self;
    char *url;
    char *status_page;
    char *string;
    int total_accesses;
    int total_kBytes;
    float cpu_load;
    int uptime;
    float req_per_sec;
    float bytes_per_sec;
    float bytes_per_req;
    int busy_workers;
    int idle_workers;

    Throwable *(*retrieve)(struct apache_server_status *self);
    Throwable *(*parse)(struct apache_server_status *self);
    Throwable *(*set_url)(struct apache_server_status *self, char *url);
    char *(*to_string)(struct apache_server_status *self);
    void (*destroy)(struct apache_server_status *self);
} ApacheServerStatus;

/*
 * ---------------------------------------------------------------------------
 * Function   : new_apache_server_status
 * Description: This function create a new instance of ApacheServerStatus.
 *
 * Param      :
 *
 * Return     : The pointer to new instance of ApacheServerStatus.
 * ---------------------------------------------------------------------------
 */
ApacheServerStatus *new_apache_server_status();

#endif