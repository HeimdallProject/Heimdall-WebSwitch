//
//============================================================================
// Name             : log.c
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This file contains all the stuffs useful in order to print / write log file.
// ===========================================================================
//

#include "../include/log.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Log
 * * ---------------------------------------------------------------------------
 */
static Log *singleton_log = NULL;

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

    int byte_read = 0;

    if (INFO_LEVEL >= (int)config->log_level) {

        char *formatted_str;

        va_list arg;
        va_start (arg, format);
        byte_read = vasprintf(&formatted_str, format, arg);
        va_end (arg);

        char *output; 
        byte_read = asprintf(&output, "%s %s D/ - %s", timestamp(), tag, formatted_str);

        free(formatted_str);

        printf("%s \n", output);
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

    int byte_read = 0;

    if (DEBUG_LEVEL >= (int)config->log_level) {

        char *formatted_str;

        va_list arg;
        va_start (arg, format);
        byte_read = vasprintf(&formatted_str, format, arg);
        va_end (arg);

        char *output; 
        byte_read = asprintf(&output, "%s %s D/ - %s", timestamp(), tag, formatted_str);

        free(formatted_str);

        printf("%s \n", output);
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

    ConfigPtr config = get_config();

    int byte_read = 0;

    if (ERROR_LEVEL >= (int)config->log_level) {
        
        char *formatted_str;

        va_list arg;
        va_start (arg, format);
        byte_read = vasprintf(&formatted_str, format, arg);
        va_end (arg);

        char *output; 
        byte_read = asprintf(&output, "%s %s D/ - %s", timestamp(), tag, formatted_str);

        free(formatted_str);

        fprintf(stderr, "%s \n", output);
    }

    return byte_read;

    //fprintf(stderr, "%s E/%s - %s\n", timestamp(), tag, msg);

    //TODO scrivere sul file se variabile è settata nel config
}

/*
 * ---------------------------------------------------------------------------
 * Function     : print_throwable
 * Description  : Used for print human-readable object Throwable.
 *
 * Param        :
 *   Throwable  : Pointer to object Throwable.
 *
 * Return       : void.
 * ---------------------------------------------------------------------------
 */
static void print_throwable(Throwable *thr) {

    if (thr->status == STATUS_OK) {
        fprintf(stdout, "Status: %d \nMessage: %s \nStack Trace: \n %s", thr->status, thr->message, thr->stack_trace);
    } else {
        fprintf(stderr, "Status: %d \nMessage: %s \nStack Trace: \n %s", thr->status, thr->message, thr->stack_trace);
    }
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
        fprintf(stderr, "Memory allocation error in new_log.\n");
        exit(EXIT_FAILURE);
    }

    // Set "methods"
    log->d = d;
    log->i = i;
    log->e = e;
    log->t = print_throwable;

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

    // return singleton
    return singleton_log;
}