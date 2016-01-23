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
 * Description  : Check if the ThrowablePtr is an error or not. If not it destroys
 *                itself.
 *
 * Param        :
 *  self        : The pointer to the Throwable.
 *
 * Return       : FALSE or TRUE.
 * ---------------------------------------------------------------------------
 */
int is_an_error_throwable(ThrowablePtr self) {
    if (self->status == STATUS_OK) {
        self->destroy(self);
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
ThrowablePtr thrown_throwable(ThrowablePtr self, char *stack_trace) {
    
    int value = asprintf(&self->stack_trace, "%s->%s", self->stack_trace, stack_trace);
    UNUSED(value); // TODO controllare il valore di ritorno

    return self;
}


void destroy_throwable(ThrowablePtr self) {
    //free(self->message);

    free(self);
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

    throwable->message     = msg;
    throwable->status      = status;
    throwable->stack_trace = stack_trace;

    // Set "methods"
    throwable->thrown      = thrown_throwable;
    throwable->is_an_error = is_an_error_throwable;
    throwable->destroy     = destroy_throwable;

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

    throwable->message     = NULL;
    throwable->status      = STATUS_OK;
    throwable->stack_trace = NULL;

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