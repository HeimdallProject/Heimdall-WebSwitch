//
//============================================================================
// Name             : throwable.c
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This file contains all the stuffs useful in order to create and manage Throwable object.
// ===========================================================================
//

#include <stdio.h>
#include <stdlib.h>
#include "log.h"

Throwable *new_throwable();

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Throwable
 * ---------------------------------------------------------------------------
 */
Throwable *singleton_throwable = NULL;

Throwable *create(int status, char *msg, char *function_name) {

    Throwable *thr = new_throwable();
    thr->message        = msg;
    thr->status         = status;

    // TODO fare funzione append in helper
    thr->stack_trace    = function_name;

    return thr;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : new_throwable
 * Description  : Alloc and initialize object Throwable.
 *
 * Param        :
 *
 * Return       : Pointer to object Throwable.
 * ---------------------------------------------------------------------------
 */
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

/*
 * ---------------------------------------------------------------------------
 * Function     : get_throwable
 * Description  : Return singleton instance of Throwable object, if this instance is not already created
 *                this function call automatically new_throwable() for instantiate the object itself.
 *
 * Param        :
 *
 * Return       : Pointer to object Throwable.
 * ---------------------------------------------------------------------------
 */
Throwable *get_throwable() {

    if (singleton_throwable == NULL) {
        singleton_throwable = new_throwable();
    }

    // return singleton
    return singleton_throwable;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main() {

    Throwable Throwable = *get_throwable();

    Log *v = get_log();
    v->print_throwable(Throwable.create(STATUS_OK, "Ciao ciccio", "main()"));

    return EXIT_SUCCESS;
}