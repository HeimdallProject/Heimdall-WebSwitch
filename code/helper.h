//
//============================================================================
// Name             : utils.h
// Author           : Claudio Pastorini
// Version          : 0.2
// Data Created     : 12/05/2015
// Last modified    : 31/05/2015
// Description      : Header file with some useful macro and functions.
// ===========================================================================
//
#ifndef HELPER_H
#define HELPER_H

#define TAG_HELPER "HELPER"  // Macro for log message

#include "throwable.h"

#define STATUS_OK     0  // Macro for status ok
#define STATUS_ERROR -1  // Macro for status error

#define TRUE  1  // Macro for true
#define FALSE 0  // Macro for false

/*
 * ---------------------------------------------------------------------------
 * Function   : str_to_int
 * Description: This function converts a string into a int value using the
 *              C command strotol.
 *
 * Param      :
 *   string:    The string to convert.
 *   value:     The pointer to result value.
 *
 * Return     : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
Throwable *str_to_int(const char *string, int *value);

/*
 * ---------------------------------------------------------------------------
 * Function   : str_to_float
 * Description: This function converts a string into a float value using the
 *              C command strtof.
 *
 * Param      :
 *   string:    The string to convert.
 *   value:     The pointer to result value.
 *
 * Return     : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
Throwable *str_to_float(char *string, float *value);

/*
 * ---------------------------------------------------------------------------
 * Function   : str_to_lower
 * Description: This function converts a string into the same string with all
 *              the chars as lowercase letters using C command tolower
 *
 * Param      :
 *   string:    The string to convert.
 *
 * Return     : None
 * ---------------------------------------------------------------------------
 */
char *str_to_lower(char *string);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_error_by_errno
 * Description: This function returns a string where there is the errorno
 *              in a human readable way.
 *
 * Param      :
 *   error      : The errno number.
 *
 * Return     : None.
 * ---------------------------------------------------------------------------
 */
// TODO capire come gestire il risultato (si fa ritornare il puntatore o ne si accetta uno?)
char *get_error_by_errno(const int error);

/*
 * ---------------------------------------------------------------------------
 * Function   : timestamp
 * Description: This function returns a string where there is the timestamp
 *              in a human readable way.
 *
 * Param      :
 *
 * Return     : The timestamp in a human readable way.
 * ---------------------------------------------------------------------------
 */
char *timestamp();

#endif