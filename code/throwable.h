//
//============================================================================
// Name             : throwable.h
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This header file contains all the stuffs useful in order to create and manage Throwable object.
// ===========================================================================
//

#define STATUS_OK     0  // Macro for status ok
#define STATUS_ERROR -1  // Macro for status error

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct throwable
 * Description  : This struct collect all functions pointers and attributes for messages.
 *
 * Functions:
 *  d           : Pointer to debug function.
 *  i           : Pointer to info function.
 *  e           : Pointer to error function.
 *
 * Data:
 *  status      : Error level, 0 or -1 see macro in this .h.
 *  message     : The error message.
 *  stack_trace : Contains all function name where error occurs.
 * ---------------------------------------------------------------------------
 */
typedef struct throwable {
    int status;
    char *message;
    char *stack_trace;
    struct throwable* (*create)(int status, char *tag, char *msg);
} Throwable;