#include "../include/log.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Log
 * * ---------------------------------------------------------------------------
 */
static Log *singleton_log = NULL;
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

    int byte_read = 0;

    int level = 2;
    //str_to_int(config->log_level, &level); TOTO settato manualmente

    int print_enable = 0;
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

    int byte_read = 0;

    int level = 2;
    //str_to_int(config->log_level, &level); TOTO settato manualmente

    int print_enable = 0;
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

    // return singleton
    return singleton_log;
}