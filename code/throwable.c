//
//============================================================================
// Name             : throwable.c
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.2
// Data Created     : 30/05/2015
// Last modified    : 31/05/2015
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

/*
 * ---------------------------------------------------------------------------
 * Function     : thrown_throwable
 * Description  : Thrown a Throwable adding into the stack_trace the name of
 *                the new function.
 *
 * Param        :
 *  self                : The pointer to the Throwable.
 *  stack_trace         : The name of the function that use this Throwable.
 *
 * Return       : Pointer to object Throwable.
 * ---------------------------------------------------------------------------
 */
Throwable* thrown_throwable(Throwable *self, char *stack_trace) {
    // TODO fare funzione append in helper
    self->message = stack_trace;
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : create_throwable
 * Description  : Create a son of singleton Throwable and put all info in.
 *
 * Param        :
 *  status              : The status code of Throwable, 0 for all ok, -1 otherwise.
 *  msg                 : The message of Throwable.
 *  stack_trace         : The name of the function that use this Throwable.
 *
 * Return       : Pointer to object Throwable.
 * ---------------------------------------------------------------------------
 */
Throwable *create_throwable(int status, char *msg, char *stack_trace) {

    Throwable *throwable = new_throwable();
    throwable->self = throwable;
    throwable->message = msg;
    throwable->status = status;

    // Set "methods"
    throwable->stack_trace = stack_trace;
    throwable->thrown = thrown_throwable;

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
