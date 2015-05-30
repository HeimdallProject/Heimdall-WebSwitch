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

Throwable *create_throwable(int status, char *msg, char *function_name) {

    Throwable *throwable = new_throwable();
    throwable->message = msg;
    throwable->status = status;

    // TODO fare funzione append in helper
    throwable->stack_trace = function_name;

    return throwable;
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

    Throwable *throwable = malloc(sizeof(Throwable));
    if (throwable == NULL) {
        fprintf(stderr, "Memory allocation error in new_throwable().\n");
        exit(EXIT_FAILURE);
    }

    // Set "methods"
    throwable->create = create_throwable;

    return throwable;
}

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
/*
int main() {

    Throwable Throwable = *get_throwable();

    Log log = *get_log();
    log.print_throwable(Throwable.create(STATUS_ERROR, "Ciao ciccio", "main()"));

    return EXIT_SUCCESS;
}*/
