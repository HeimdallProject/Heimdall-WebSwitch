//
//============================================================================
// Name             : log.c
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This file contains all the stuffs useful in order instantiate the object Throwable
// ===========================================================================
//

#include <stdio.h>
#include <stdlib.h>
#include "log.h"

Throwable *new_throwable();

Throwable *singleton_throwable = NULL;

Throwable *create(int status, char *msg, char *function_name) {

    Throwable *thr = new_throwable();
    thr->message        = msg;
    thr->status         = status;

    // TODO fare funzione append in helper
    thr->stack_trace    = function_name;

    return thr;
}

Throwable *new_throwable() {

    Throwable *thr = malloc(sizeof(Throwable));
    if (thr == NULL) {
        fprintf(stderr, "Memory allocation error in new_throwable().\n");
        exit(EXIT_FAILURE);
    }

    // Set "methods"
    thr->create = create;

    return thr;
}

Throwable *get_throwable() {

    if (singleton_throwable == NULL) {
        singleton_throwable = new_throwable();
    }

    // return singleton
    return singleton_throwable;
}


int main() {

    Throwable Throwable = *get_throwable();

    Log *v = get_log();
    v->print_throwable(Throwable.create(STATUS_OK, "Ciao ciccio", "main()"));

    return EXIT_SUCCESS;
}