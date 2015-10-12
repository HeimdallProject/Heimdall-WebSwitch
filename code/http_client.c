//
//============================================================================
// Name       : connection.c
// Author     : Claudio Pastorini and Alessio Moretti
// Version    : 0.1
// Description: simple HTTP client
// ============================================================================
//

#include "http_client.h"
#include "log.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : set_request_http_client
 * Description: This function sets request struct to the HTTP Client.
 *
 * Param      :
 *   self   : The pointer to the HTTPClient.
 *   req    : The pointer to the HTTPRequest.
 *
 * Return     : A Throwable.
 * ---------------------------------------------------------------------------
 */
Throwable *set_request_http_client(HTTPClient *self, HTTPRequest *req) {
    if (req != null) {
        // Set request
        self->request = req;
    } else {
        return (*get_throwable()).create(STATUS_ERROR, "There is no HTTPRequest attached", "set_request_http_client");
    }
    return (*get_throwable()).create(STATUS_OK, NULL, "set_request_http_client");
}

/*
 * ---------------------------------------------------------------------------
 * Function   : get_request_http_client
 * Description: This function returns the request bound to this instance of HTTPClient.
 *
 * Param      :
 *   self   : The pointer to the HTTPClient.
 *
 * Return     : HTTPRequest bound to this HTTPClient.
 * ---------------------------------------------------------------------------
 */
HTTPRequest *get_request_http_client(HTTPClient *self) {
    return self->request;
}
/*
*//*
 * ---------------------------------------------------------------------------
 * Function   : get_response_http_client
 * Description: This function returns the response bound to this instance of HTTPClient.
 *
 * Param      :
 *   self   : The pointer to the HTTPClient.
 *
 * Return     : HTTPResponse bound to this HTTPClient.
 * ---------------------------------------------------------------------------
 *//*
HTTPResponse *get_response_http_client(HTTPClient *self) {
    return self->response;
}*/

/*
 * ---------------------------------------------------------------------------
 * Function   : make_request_http_client
 * Description: This function free all the memory bound with HTTPClient.
 *
 * Param      :
 *   self   : The pointer to the HTTPClient.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void make_request_http_client(HTTPClient *self) {
}

/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_http_client
 * Description: This function free all the memory bound with HTTPClient.
 *
 * Param      :
 *   self   : The pointer to the HTTPClient.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
void destroy_http_client(void *self) {
    free(((HTTPClient *) self)->request);           // Free space for request
    //free((HTTPClient *) self)->response);         // Free space for response
    free(self);                                     // Free all the struct
}

HTTPClient *new_http_client() {

    HTTPClient *http_client = malloc(sizeof(HTTPClient));
    if (http_client == NULL) {
        (*get_log()).e(TAG_HTTP_CLIENT, "Memory allocation error in new_http_client.\n");
        exit(EXIT_FAILURE);
    }
    // Set self linking
    http_client->self = http_client;

    // Set "methods"
    http_client->set_request = set_request_http_client;
    http_client->get_request = get_request_http_client;
    //http_client->get_response = get_response_http_client;
    http_client->make_request = make_request_http_client;
    http_client->destroy = destroy_http_client;

    return apache_server_status;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {

    return EXIT_SUCCESS;
}