//
//============================================================================
// Name             : throwable.h
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.2
// Data Created     : 30/05/2015
// Last modified    : 01/06/2015
// Description      : This header file contains all the stuffs useful in order to create and manage Throwable object.
// ===========================================================================
//
#ifndef THROWABLE_H
#define THROWABLE_H

#define TAG_THROWABLE "THROWABLE"  // Macro for log message

#define STATUS_OK     0  // Macro for status ok
#define STATUS_ERROR -1  // Macro for status error

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
} Throwable;

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
Throwable *get_throwable();

#endif