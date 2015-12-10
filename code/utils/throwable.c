#include "../include/throwable.h"

/*
 *  See .h for more information.
 */
ThrowablePtr new_throwable();

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Throwable
 * ---------------------------------------------------------------------------
 */
ThrowablePtr singleton_throwable = NULL;

/*
 * ---------------------------------------------------------------------------
 * Function     : is_an_error_throwable
 * Description  : Check if the ThrowablePtr is an error or not.
 *
 * Param        :
 *  self        : The pointer to the Throwable.
 *
 * Return       : FALSE or TRUE.
 * ---------------------------------------------------------------------------
 */
int is_an_error_throwable(ThrowablePtr self) {
    if (self->status == STATUS_OK) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function     : thrown_throwable
 * Description  : Thrown a ThrowablePtr adding into the stack_trace the name of
 *                the new function.
 *
 * Param        :
 *  self                : The pointer to the Throwable.
 *  stack_trace         : The name of the function that use this Throwable.
 *
 * Return       : Pointer to object Throwable.
 * ---------------------------------------------------------------------------
 */
// TODO creare funzione strlen per fare malloc intelligente
ThrowablePtr thrown_throwable(ThrowablePtr self, char *stack_trace) {
    char *old_stack_trace = self->stack_trace;

    char *new_stack_trace = malloc(1024);
    if (new_stack_trace == NULL) {
        return self;
    }
    snprintf(new_stack_trace, 1024, "%s->%s", old_stack_trace, stack_trace);

    // Free the old string
    //free(old_stack_trace);  // TODO non funziona... voglio pulire spazio vecchia stringa ma non me lo fa fare

    self->stack_trace = new_stack_trace;
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : create_throwable
 * Description  : Create a son of singleton ThrowablePtr and put all info in.
 *
 * Param        :
 *  status              : The status code of Throwable, 0 for all ok, -1 otherwise.
 *  msg                 : The message of Throwable.
 *  stack_trace         : The name of the function that use this Throwable.
 *
 * Return       : Pointer to object Throwable.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr create_throwable(int status, char *msg, char *stack_trace) {

    ThrowablePtr throwable = new_throwable();

    throwable->message = msg;
    throwable->status = status;
    throwable->stack_trace = stack_trace;

    // Set "methods"
    throwable->thrown = thrown_throwable;
    throwable->is_an_error = is_an_error_throwable;

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
ThrowablePtr new_throwable() {

    ThrowablePtr throwable = malloc(sizeof(Throwable));
    if (throwable == NULL) {
        fprintf(stderr, "Memory allocation error in new_throwable!");
        exit(EXIT_FAILURE);
    }

    // Set "methods"
    throwable->create = create_throwable;

    return throwable;
}

/*
 *  See .h for more information
 */
ThrowablePtr get_throwable() {

    if (singleton_throwable == NULL) {
        singleton_throwable = new_throwable();
    }

    // return singleton
    return singleton_throwable;
}