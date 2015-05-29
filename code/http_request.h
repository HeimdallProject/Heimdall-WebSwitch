//
//============================================================================
// Name       : http_request.h
// Author     : Alessio Moretti
// Version    : 0.1
// Description: Header file for the HTTP request header parsing - it includes all
//              the structs and functions useful to parse the header of the HTTP
//              request in order to let the web switch balancing algorithm to
//              decide the  machines load.
//              Further infos at:
//              - http://tools.ietf.org/html/rfc7230
//              - http://tools.ietf.org/html/rfc7231
//              - http://tools.ietf.org/html/rfc7232
//              - http://tools.ietf.org/html/rfc7233
//              - http://tools.ietf.org/html/rfc7234
//              - http://tools.ietf.org/html/rfc7235
//              - http://en.wikipedia.org/wiki/List_of_HTTP_header_fields (dev)
// ============================================================================
//

/*
 * this macros will be used to retrieve the necessary info from the HTTP request
 * along as the maximum header field
 */
#define MAX_HEADER              32768
#define HEADER_ALLOC_UNIT       128
#define REQ_UNIT                4
#define RESOURCE_LOC_UNIT       128
#define PROTOCOL                "HTTP/1.1"
#define KNOWN                   1
#define UNKNOWN                 0
/*
 -----------------------------------------------------------------------------
 *
 *                       1.   REQUEST CONTENT MACROS
 *
 -----------------------------------------------------------------------------
 */
#define ACCEPT                  "accept"
#define ACCEPT_CHAR             "accept-aharset"
#define ACCEPT_ENC              "accept-encoding"
#define ACCEPT_LANG             "accept-language"
#define ACCEPT_DATE             "accept-datetime"
#define CONTENT_LEN             "content-length"
#define CONTENT_MD5             "content-md5"
#define CONTENT_TYPE            "content-type"

/*
 -----------------------------------------------------------------------------
 *
 *                        2.   CLIENT MACROS
 *
 -----------------------------------------------------------------------------
 */
#define FROM                    "from"
#define HOST                    "host"
#define USER_AGENT              "user-agent"


/*
 -----------------------------------------------------------------------------
 *
 *                      3.     ROUTING CONTROL MACROS
 *
 -----------------------------------------------------------------------------
 */
#define CACHE_CTRL              "cache-control"
#define CONNECTION              "connection"
#define COOKIE                  "cookie"
#define DATE                    "date"
#define MODIFIED_SINCE          "if-modified-since"
#define MAX_FORWARDS            "max-forwards"

/*
 -----------------------------------------------------------------------------
 *
 *                       4.     NETWORK SECURITY MACROS
 *
 -----------------------------------------------------------------------------
 */
#define UPGRADE                 "upgrade"
#define VIA                     "via"
#define WARNING                 "warning"

/*
 * this struct will be used to retrieve the request headers info desired
 */
// TODO make it a type (and if we want a pointer to struct in order to hide itself)
struct http_request {
    char *status;                                       // whether the request can be handled
    char *req_type;                                     // type of the request
    char *req_protocol;                                 // accepted only HTTP/1.1
    char *req_resource;                                 // resource locator
    char *req_accept;                                   // accepting content infos
    char *req_from;                                     // client and request generic infos
    char *req_host;
    char *req_content_type;                             // content type info
    char *req_content_len;
    char *req_upgrade;                                  // no protocol upgrade are allowed
};


/*
 * ---------------------------------------------------------------------------
 * Function   : get_header
 * Description: This function is used to find which header line is currently
 *              read and if it is useful for the web switch to analyze its
 *              contents (balancing algorithm purpose)
 *
 * Param      :
 *   string: the line of the HTTP header currently read by the web switch
 *
 * Return     :
 *   struct http_request
 * ---------------------------------------------------------------------------
 */
struct http_request *get_header(char *req_line, struct http_request *http);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_request
 * Description: This function is used to find which request has been performed
 *
 * Param      :
 *   string: the line of the HTTP request header currently read by the web switch
 *
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
struct http_request *get_request(char *req_line, struct http_request *http, int len);

/*
 * ---------------------------------------------------------------------------
 * Function   : read_request_headers
 * Description: This function will be used to read the request headers.
 *              Pay attention: it is based this HTTP request format:
 *
 *              METHOD RESOURCE HTTP/1.1
 *              HEADER_1: PARAMS
 *              ...
 *              HEADER_N: PARAMS
 *              [ blank line ]
 *
 * Param      :
 *   buffer:  the buffer into which has been stored the request
 *
 * Return     :
 *   struct http_request pointer
 * ---------------------------------------------------------------------------
 */
struct http_request *read_request_headers(char *buffer, struct http_request *http);