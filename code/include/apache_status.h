//
//============================================================================
// Name        : apache_status.h
// Description : This header file contains all the stuffs useful in order to
//               reads the status of an Apache server.
//               For other info read:
//                  - http://httpd.apache.org/docs/2.4/mod/mod_status.html
// ===========================================================================
//
#ifndef APACHE_STATUS_H
#define APACHE_STATUS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "../include/helper.h"
#include "../include/connection.h"
#include "../include/log.h"
#include "../include/throwable.h"

#define TAG_APACHE_STATUS "APACHE_STATUS"

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
    // Initialize server_status
    ApacheServerStatusPtr server_status = new_apache_server_status();
    server_status->set_url(server_status, "www.laziobus.it");

    // Retrieve status
    ThrowablePtr throwable = server_status->retrieve(server_status);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    // Parse
    throwable = server_status->parse(server_status);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        exit(EXIT_FAILURE);
    }

    get_log()->i(TAG_APACHE_STATUS, server_status->to_string(server_status));
    // Destroy the object
    server_status->destroy(server_status);

    return EXIT_SUCCESS;
 */

/*
 * ---------------------------------------------------------------------------
 * Structure   : apache_server_status
 * Description : This struct collects all data about the state of a Apache server.
 *
 * Data        :
 *  url            : The URL of the server status page.
 *  status_page    : The complete status page.
 *  total_accesses : A total number of accesses served.
 *  total_kBytes   : A total number of byte count served.
 *  cpu_load       : The current percentage CPU used by each worker and in total
 *                   by all workers combined
 *  uptime         : The time the server was started/restarted and the time it
 *                   has been running for.
 *  req_per_sec    : Averages giving the number of requests per second, the
 *                   number of bytes served per second and the average number
 *                   of bytes per request.
 *  bytes_per_sec  : The number of bytes served per second.
 *  bytes_per_req  : The average number of bytes per request.
 *  busy_workers   : The number of worker serving requests.
 *  idle_workers   : The number of idle worker.
 *
 * Functions   :
 *  retrieve : Pointer to retrieve_apache_status function.
 *  parse    : Pointer to parse_apache_status function.
 *  set_url  : Pointer to set_url function.
 *  destroy  : Pointer to destroy function.
 */
typedef struct apache_server_status {
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

    ThrowablePtr (*retrieve)(struct apache_server_status *self);
    ThrowablePtr (*parse)(struct apache_server_status *self);
    ThrowablePtr (*set_url)(struct apache_server_status *self, char *url);
    void (*destroy)(struct apache_server_status *self);
} ApacheServerStatus, *ApacheServerStatusPtr;

/*
 * ---------------------------------------------------------------------------
 * Function    : new_apache_server_status
 * Description : This function create a new instance of ApacheServerStatus.
 *
 * Param       : None
 *
 * Return      : The pointer to new instance of ApacheServerStatus.
 * ---------------------------------------------------------------------------
 */
ApacheServerStatusPtr new_apache_server_status();

#endif //APACHE_STATUS_H