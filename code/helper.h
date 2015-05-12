//
//============================================================================
// Name       : utils.h
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: Header file with some useful macro and functions.
// ===========================================================================
//

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#define STATUS_OK     0  // Macro for status ok
#define STATUS_ERROR -1  // Macro for status error

#define TRUE  1  // Macro for true
#define FALSE 0  // Macro for false

// TODO define a way to return message error
#define VERBOSE_LEVEL 2  // Macro for verbose message level
#define DEBUG_LEVEL   3  // Macro for debug message level
#define INFO_LEVEL    4  // Macro for info message level
#define WARN_LEVEL    5  // Macro for warn message level
#define ERROR_LEVEL   6  // Macro for error message level

/*
 *
 */
struct message {
    int code;
    int level;
    char *message;
};
/*
 * ---------------------------------------------------------------------------
 * Function   : str_to_int
 * Description: This function converts a string into a int value using the
 *              C command strotol.
 *
 * Param      :
 *   string:    The string to convert.
 *
 * Return     : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
int str_to_int(char *string) {

    char *pointer;
    int value;

    errno = 0;
    value = (int) strtol(string, &pointer, 0);
    if (errno != 0 || *pointer != '\0') {
        fprintf(stderr, "Invalid number\n");
        return STATUS_ERROR;
    }

    return value;
}

/*
 * ---------------------------------------------------------------------------
 * Function   : str_to_float
 * Description: This function converts a string into a float value using the
 *              C command strotof.
 *
 * Param      :
 *   string:    The string to convert.
 *
 * Return     : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
float str_to_float(char *string) {

    char *pointer;
    float value;

    errno = 0;
    value = strtof(string, &pointer);
    if (errno != 0 || *pointer != '\0') {
        fprintf(stderr, "Invalid number\n");
        return STATUS_ERROR;
    }

    return value;
}
