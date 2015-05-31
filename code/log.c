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

#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "helper.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Log
 * * ---------------------------------------------------------------------------
 */
Log *singleton_log = NULL;

/*
 * ---------------------------------------------------------------------------
 * Function     : i
 * Description  : Used for print and/or write information messages to log.
 *
 * Param        :
 *   tag        : String that indicates where this log.
 *   msg        : Information message.
 *
 * Return       : void.
 * ---------------------------------------------------------------------------
 */
void i(char* tag, char *msg) {

    if (INFO_LEVEL >= PRINT_LEVEL) {
        fprintf(stdout, "%s I/%s - %s\n", timestamp(), tag, msg);
    }

    //TODO scrivere sul file se variabile è settata nel config
}

/*
 * ---------------------------------------------------------------------------
 * Function     : d
 * Description  : Used for print and/or write debug messages to log.
 *
 * Param        :
 *   tag        : String that indicates where this log.
 *   msg        : Debug message.
 *
 * Return       : void.
 * ---------------------------------------------------------------------------
 */
void d(char* tag, char *msg) {

    if (DEBUG_LEVEL >= PRINT_LEVEL) {
        fprintf(stdout, "%s D/%s - %s\n", timestamp(), tag, msg);
    }

    //TODO scrivere sul file se variabile è settata nel config
}

/*
 * ---------------------------------------------------------------------------
 * Function     : e
 * Description  : Used for print and/or write error messages to log.
 *
 * Param        :
 *   tag        : String that indicates where this log.
 *   msg        : Error message.
 *
 * Return       : void.
 * ---------------------------------------------------------------------------
 */
void e(char* tag, char *msg) {

    if (ERROR_LEVEL >= PRINT_LEVEL) {
        fprintf(stderr, "%s E/%s - %s\n", timestamp(), tag, msg);
    }

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
void print_throwable(Throwable *thr) {

    if (thr->status == STATUS_OK) {
        fprintf(stdout, "Status %d \nMessage: %s \nStack Trace: \n %s", thr->status, thr->message, thr->stack_trace);
    } else {
        fprintf(stderr, "Status %d \nMessage: %s \nStack Trace: \n %s", thr->status, thr->message, thr->stack_trace);
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

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
/*int main() {

    Log log = *get_log();
    log.i("TAG", "INFO");
    log.d("TAG", "DEBUG");
    log.e("TAG", "ERRORE");
}*/