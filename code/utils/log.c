#include "../include/log.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Log
 * * ---------------------------------------------------------------------------
 */
static Log *singleton_log = NULL;

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of log file pointer
 * * ---------------------------------------------------------------------------
 */


//TODO use a more low level API in order to avoid swap printing
/*
 * ---------------------------------------------------------------------------
 * Function     : i
 * Description  : Used for print and/or write information messages to log.
 *                Use it like a printf.
 *
 * Param        :
 *   tag        : String that indicates where this log.
 *   msg        : Information message.
 *   ...        : params.
 *
 * Return       : If successful, the total number of characters written is returned 
 *                otherwise a negative number is returned.
 * ---------------------------------------------------------------------------
 */
static int i(const char* tag, const char *format, ...) {

    ConfigPtr config = get_config();
    UNUSED(config);
    UNUSED(tag);
    UNUSED(format);

    int byte_read = 0;

    int level = 2;
    //str_to_int(config->log_level, &level); TODO settato manualmente

    int print_enable = 1;
    //str_to_int(config->print_enable, &print_enable);

    if (INFO_LEVEL >= level && print_enable == 1) {

        char *formatted_str;

        va_list arg;
        va_start (arg, format);
        byte_read = vasprintf(&formatted_str, format, arg);
        va_end (arg);

        char *output;
        byte_read = asprintf(&output, "%s: I/%s: %s", timestamp(), tag, formatted_str);

        free(formatted_str);

        printf("%s \n", output);
        fflush(stdout);
        free(output);
    }

    return byte_read;
    //TODO scrivere sul file se variabile è settata nel config
}

/*
 * ---------------------------------------------------------------------------
 * Function     : d
 * Description  : Used for print and/or write debug messages to log.
 *                Use it like a printf.
 *
 * Param        :
 *   tag        : String that indicates where this log.
 *   msg        : Information message.
 *   ...        : params.
 *
 * Return       : If successful, the total number of characters written is returned 
 *                otherwise a negative number is returned.
 * ---------------------------------------------------------------------------
 */
static int d(const char* tag, const char *format, ...) {

    //TODO scrivere sul file se variabile è settata nel config
    ConfigPtr config = get_config();
    UNUSED(config);
    UNUSED(tag);
    UNUSED(format);

    int byte_read = 0;

    int level = 2;
    //str_to_int(config->log_level, &level); TOTO settato manualmente

    int print_enable = 1;
    //str_to_int(config->print_enable, &print_enable);

    if (DEBUG_LEVEL >= level && print_enable == 1) {

        char *formatted_str;

        va_list arg;
        va_start (arg, format);
        byte_read = vasprintf(&formatted_str, format, arg);
        va_end (arg);

        char *output; 
        byte_read = asprintf(&output, "%s: D/%s: %s", timestamp(), tag, formatted_str);

        free(formatted_str);

        printf("%s \n", output);
        fflush(stdout);
        free(output);
    }
    return byte_read;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : e
 * Description  : Used for print and/or write error messages to log.
 *                Use it like a printf.
 *
 * Param        :
 *   tag        : String that indicates where this log.
 *   msg        : Information message.
 *   ...        : params.
 *
 * Return       : If successful, the total number of characters written is returned 
 *                otherwise a negative number is returned.
 * ---------------------------------------------------------------------------
 */
static int e(const char* tag, const char *format, ...) {

    //TODO ma farla globale?
    ConfigPtr config = get_config();
    UNUSED(config);
    UNUSED(tag);
    UNUSED(format);

    int byte_read = 0;


    int level = 2;
    //str_to_int(config->log_level, &level); TOTO settato manualmente

    int print_enable = 1;
    //str_to_int(config->print_enable, &print_enable);

    if (ERROR_LEVEL >= level && print_enable == 1) {
        
        char *formatted_str;

        va_list arg;
        va_start (arg, format);
        byte_read = vasprintf(&formatted_str, format, arg);
        va_end (arg);

        char *output; 
        byte_read = asprintf(&output, "%s: E/%s: %s", timestamp(), tag, formatted_str);

        free(formatted_str);

        fprintf(stderr, "%s \n", output);
        fflush(stderr);
        free(output);
    }


    return byte_read;

    //fprintf(stderr, "%s E/%s - %s\n", timestamp(), tag, msg);

    //TODO scrivere sul file se variabile è settata nel config
}

/*
 * ---------------------------------------------------------------------------
 * Function     : r
 * Description  : Used for print human-readable request logging
 *
 * Param        : int type: RQST or RESP
 *                void *arg: it can be an HTTPRequestPtr (RQST type) or an
 *                HTTPResponsePtr (RESP type)
 *                const char *remote: the machine to which has been forwarded
 *                the request (log req) and from which we are waiting for a response 
 *                (log resp)
 *
 * Return       : If successful, the total number of characters written is returned
 *                otherwise a negative number is returned.
 * ---------------------------------------------------------------------------
 */
static int r(int type, void *arg, char *remote) {

    ConfigPtr config = get_config();
    UNUSED(config);

    int byte_write = 0;

    int print_enable = 1;
    //str_to_int(config->print_enable, &print_enable); TODO settato manualmente

    if (print_enable == 1) {

        char *line;

        // handling request logging routine
        if (type == RQST) {

            // initializing file pointer
            FILE *req_log;
            // retrieving arg
            HTTPRequestPtr request = arg;

            // retrieving log file pointer or allocating it
            req_log = fopen("/vagrant/log/heimdall_req.log", "a");
            if (req_log == NULL) {
                fprintf(stderr, "Error in log file opening!\n");
            }
            // formatting log line
            byte_write = asprintf(&line, "[%s] - %s to: %s  - -  %s %s %s\n",
                                  timestamp(),
                                  request->req_host,
                                  remote,
                                  request->req_type,
                                  request->req_resource,
                                  request->req_protocol);

            byte_write = (int) fwrite(line, sizeof(char), strlen(line), req_log);
            fflush(req_log);
        }


        // handling response logging routine
        if (type == RESP) {

            // initializing file pointer
            FILE *resp_log;
            // retrieving args
            HTTPRequestPtr response = ((HTTPResponsePtr) arg)->response;

            // retrieving log file pointer or allocating it
            resp_log = fopen("/vagrant/log/heimdall_resp.log", "a");
            if (resp_log == NULL) {
                fprintf(stderr, "Error in log file opening!\n");
            }
            // formatting log line
            byte_write = asprintf(&line, "[%s] - %s response to Heimdall: %s %s %s\n",
                                  timestamp(),
                                  remote,
                                  response->req_protocol,
                                  response->resp_code,
                                  response->resp_msg);

            byte_write = (int) fwrite(line, sizeof(char), strlen(line), resp_log);
            fflush(resp_log);
        }

    }

    return byte_write;

}

/*
 * ---------------------------------------------------------------------------
 * Function     : t
 * Description  : Used for print human-readable object Throwable.
 *
 * Param        :
 *   ThrowablePtr  : Pointer to object Throwable.
 *
 * Return       : void.
 * ---------------------------------------------------------------------------
 */
static void t(ThrowablePtr thr) {
    e("Throwable", "\nStatus: %d \nMessage: %s \nStack Trace: \n %s\n", thr->status, thr->message, thr->stack_trace);
    thr->destroy(thr);
}



/*
 * ---------------------------------------------------------------------------
 * Function     : new_log
 * Description  : Alloc and initialize object Log.
 *
 * Param        :
 *
 * Return       : Pointer to object Log.
 * ---------------------------------------------------------------------------
 */
Log *new_log() {

    Log *log = malloc(sizeof(Log));
    if (log == NULL) {
        fprintf(stderr, "Memory allocation error in new_log!");
        exit(EXIT_FAILURE);
    }

    // Set "methods"
    log->d = d;
    log->i = i;
    log->e = e;
    log->r = r;
    log->t = t;

    return log;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : get_log
 * Description  : Return singleton instance of Log object, if this instance is not already created
 *                this function call automatically new_log() for instantiate the object itself.
 *
 * Param        :
 *
 * Return       : Pointer to object Log.
 * ---------------------------------------------------------------------------
 */
Log *get_log() {

    if (singleton_log == NULL) {
        singleton_log = new_log();
    }

    // initializing file pointers
    FILE *req_log;
    FILE *resp_log;

    req_log = fopen("/vagrant/log/heimdall_req.log", "a");
    if (req_log == NULL) {
        fprintf(stderr, "Error in log file opening!\n");
        exit(EXIT_FAILURE);
    }

    resp_log = fopen("/vagrant/log/heimdall_resp.log", "a");
    if (resp_log == NULL) {
        fprintf(stderr, "Error in log file opening!\n");
        exit(EXIT_FAILURE);
    }

    // return singleton
    return singleton_log;
}