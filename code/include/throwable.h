//
//============================================================================
// Name             : throwable.h
// Description      : This header file contains all the stuffs useful in order to create and manage Throwable object.
// ===========================================================================
//
#ifndef THROWABLE_H
#define THROWABLE_H

#include <stdio.h>
#include <stdlib.h>

#include "../include/macro.h"

#define TAG_THROWABLE "THROWABLE"

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
 *
 *  ThrowablePtr thr = get_throwable();
 *  LogPtr log = get_log();
 *  log->t(thr->create(STATUS_ERROR, "Ciao", "main()"));
 *
 */


/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct throwable
 * Description  : This struct collect all functions pointers and attributes for messages.
 *
 * Data:
 *  status      : Error level, 0 or -1 see macro in this .h.
 *  message     : The error message.
 *  stack_trace : Contains all function name where error occurs.
 *
 * Functions:
 *  create  : Pointer to create_throwable function.
 * ---------------------------------------------------------------------------
 */
typedef struct throwable {
    struct throwable* self;
    int status;
    char *message;
    char *stack_trace;

    struct throwable* (*create)(int status, char *msg, char *stack_trace);
    struct throwable* (*thrown)(struct throwable* self, char *stack_trace);
    int (*is_an_error)(struct throwable* self);
} Throwable, *ThrowablePtr;

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
ThrowablePtr get_throwable();

#endif //THROWABLE_H