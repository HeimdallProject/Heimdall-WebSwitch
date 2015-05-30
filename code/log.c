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

Log *singleton_log = NULL;

void i(char* tag, char *msg) {

    if (INFO_LEVEL >= PRINT_LEVEL) {
        fprintf(stdout, "%s I/%s - %s\n", timestamp(), tag, msg);
    }

    //TODO scrivere sul file se variabile è settata nel config
}

void d(char* tag, char *msg) {

    if (DEBUG_LEVEL >= PRINT_LEVEL) {
        fprintf(stdout, "%s D/%s - %s\n", timestamp(), tag, msg);
    }

    //TODO scrivere sul file se variabile è settata nel config
}

void e(char* tag, char *msg) {

    if (ERROR_LEVEL >= PRINT_LEVEL) {
        fprintf(stderr, "%s E/%s - %s\n", timestamp(), tag, msg);
    }

    //TODO scrivere sul file se variabile è settata nel config
}

void print_throwable (Throwable *thr) {

    if (thr->status == STATUS_OK) {
        fprintf(stdout, "Status %d \n Message: %s \n, Stack Trace: \n %s", thr->status, thr->message, thr->stack_trace);
    } else {
        fprintf(stderr, "Status %d \n Message: %s \n, Stack Trace: \n %s", thr->status, thr->message, thr->stack_trace);
    }
}

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
    log->print_throwable = print_throwable;

    return log;
}

Log *get_log() {

    if (singleton_log == NULL) {
        singleton_log = new_log();
    }

    // return singleton
    return singleton_log;
}

/*int main() {

    Log log = *get_log();
    log.i("TAG", "INFO");
    log.d("TAG", "DEBUG");
    log.e("TAG", "ERRORE");
}*/