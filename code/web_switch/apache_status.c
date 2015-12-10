#include "../include/apache_status.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : get_data
 * Description: This function parses a line of ApacheStatus page and put data
 *              into ApacheServerStatus struct.
 *
 * Param      :
 *   self       : The pointer to the ApacheServerStatus.
 *   line       : The line to parse
 *
 * Return     : A Throwable.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr get_data(ApacheServerStatusPtr self, char *line) {
    char delimiter = ':';
    char *text = NULL;
    char *text_data = NULL;
    int i;
    for (i = 0; i < (int) strlen(line); i++) {
        if (line[i] == delimiter) {
            line[i] = '\0';
            text = str_to_lower(line);
            text_data = line + i + 2;
            break;
        }
    }

    ThrowablePtr throwable = NULL;
    // following a comparison between texts which can be useful for perform
    // a request (forwarding packets is the final goal)
    if (strcmp(text, "total accesses") == 0) {
        throwable = str_to_int(text_data, &(self->total_accesses));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.total_access");
        }
    }

    if (strcmp(text, "total kbytes") == 0) {
        throwable = str_to_int(text_data, &(self->total_kBytes));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.total_kBytes");
        }
    }

    if (strcmp(text, "cpuload") == 0) {
        throwable = str_to_float(text_data, &(self->cpu_load));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.cpu_load");
        }
    }

    if (strcmp(text, "uptime") == 0) {
        throwable = str_to_int(text_data, &(self->uptime));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.uptime");
        }
    }

    if (strcmp(text, "reqpersec") == 0) {
        throwable = str_to_float(text_data, &(self->req_per_sec));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.req_per_sec");
        }
    }
    if (strcmp(text, "bytespersec") == 0) {
        throwable = str_to_float(text_data, &(self->bytes_per_sec));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.bytes_per_sec");
        }
    }

    if (strcmp(text, "bytesperreq") == 0) {
        throwable = str_to_float(text_data, &(self->bytes_per_req));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.bytes_per_req");
        }
    }

    if (strcmp(text, "busyworkers") == 0) {
        throwable = str_to_int(text_data, &(self->busy_workers));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.busy_workers");
        }
    }

    if (strcmp(text, "idleworkers") == 0) {
        throwable = str_to_int(text_data, &(self->idle_workers));
        if (throwable->is_an_error(throwable)) {
            return throwable->thrown(throwable, "get_data.idle_workers");
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "get_data");
}

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
ThrowablePtr parse_apache_status(ApacheServerStatusPtr self) {
    char *buffer = malloc(sizeof(char) * (strlen(self->status_page) + 1));
    if (buffer == NULL) {
        get_throwable()->create(STATUS_ERROR, "Memory allocation error", "parse_apache_status");
    }
    strcpy(buffer, self->status_page);

    char endline = '\n';
    int start = 0;
    int i;
    for (i = 0; buffer[i]; i++) {

        if (buffer[i] == endline) {
            buffer[i] = '\0';

            ThrowablePtr throwable;
            throwable = get_data(self, buffer + start);
            if (throwable->is_an_error(throwable))
                return throwable->thrown(throwable, "read_headers");
            start = i + 1;

            if ((buffer[i + 1] == endline) || (buffer[i + 1] == '\0'))
                break;
        }
    }

    return get_throwable()->create(STATUS_OK, NULL, "parse_apache_status");
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
void destroy_apache_status(ApacheServerStatusPtr self) {
    free(self->url);           // Free space for URL
    free(self->status_page);   // Free space for status page
    free(self->string);        // Free space for to_string
    free(self);                // Free all the struct
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
ThrowablePtr retrieve_apache_status(ApacheServerStatusPtr self) {
    //TODO: return NULL if no server response or no apache-status enabled
    // Initializes the new http request
    HTTPRequestPtr http_request = new_http_request();

    // Sets a new simple request for apache status
    http_request->set_simple_request(http_request, "GET", "/server-status?auto", "HTTP/1.1", self->url);

    // Resolves ip from hostname
    char ip[16];
    ThrowablePtr throwable = hostname_to_ip(self->url, ip);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Creates a new client
    int sockfd;
    throwable = create_client_socket(TCP, ip, 80, &sockfd);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Generates the simple request
    char *message;
    throwable = http_request->make_simple_request(http_request, &message);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Sends request
    throwable = send_request(&sockfd, message);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Prepares in order to receive the response
    char *response = (char *) malloc(sizeof(char) * 1024);
    if (response == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Allocation error", "retrieve_apache_status");
    }

    // Receives the response
    throwable = receive_response(&sockfd, response);
    if(throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "retrieve_apache_status");
    }

    // Closes the connection
    close_connection(sockfd);

    // Parse the response
    HTTPResponsePtr http_response = new_http_response();
    http_response->get_http_response(http_response, response);

    self->status_page = strdup(http_response->http_response_body);
    return get_throwable()->create(STATUS_OK, NULL, "retrieve_apache_status");
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
 * Return     : A Throwable.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr set_url_apache_status(ApacheServerStatusPtr self, char *url) {
    errno = 0;
    char *new_url = strdup(url);
    if (errno != 0 || new_url == NULL) return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "set_url_apache_status");
    // Set url
    self->url = new_url;

    return get_throwable()->create(STATUS_OK, NULL, "set_url_apache_status");
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
char *to_string_apache_status(ApacheServerStatusPtr self) {
    char *string;

    if (self->string != NULL) {
        free(self->string);
    }

    string = malloc(sizeof(char) * 64000);  // TODO i do not know how many space
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

ApacheServerStatusPtr new_apache_server_status() {

    ApacheServerStatusPtr apache_server_status = malloc(sizeof(ApacheServerStatus));
    if (apache_server_status == NULL) {
        get_log()->e(TAG_APACHE_STATUS, "Memory allocation error in new_apache_server_status!");
        exit(EXIT_FAILURE);
    }

    // Set "attributes" to default values
    apache_server_status->url          = NULL;
    apache_server_status->status_page  = NULL;
    apache_server_status->string       = NULL;
    apache_server_status->total_accesses  = -1;
    apache_server_status->total_kBytes    = -1;
    apache_server_status->cpu_load        = -1;
    apache_server_status->uptime          = -1;
    apache_server_status->req_per_sec     = -1;
    apache_server_status->bytes_per_sec   = -1;
    apache_server_status->bytes_per_req   = -1;
    apache_server_status->busy_workers    = -1;
    apache_server_status->idle_workers    = -1;

    // Set "methods"
    apache_server_status->retrieve = retrieve_apache_status;
    apache_server_status->parse = parse_apache_status;
    apache_server_status->destroy = destroy_apache_status;
    apache_server_status->set_url = set_url_apache_status;
    apache_server_status->to_string = to_string_apache_status;

    return apache_server_status;
}