//
//============================================================================
// Name       : helper.h
// Description: Header file with some useful macro and functions.
// ===========================================================================
//
#ifndef HELPER_H
#define HELPER_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#include "../include/throwable.h"
#include "../include/log.h"
#include "../include/macro.h"

#define TAG_HELPER "HELPER"

/*
 * ---------------------------------------------------------------------------
 * Function   	: str_to_int
 * Description	: This function converts a string into a int value using the
 *              	C command strotol.
 *
 * Param      	:
 *   string 	: The string to convert.
 *   value 		: The pointer to result value.
 *
 * Return     	: The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr str_to_int(const char *string, int *value);

/*
 * ---------------------------------------------------------------------------
 * Function   	: str_to_long
 * Description	: This function converts a string into a long value using the
 *              	C command strotol.
 *
 * Param      	:
 *   string 	: The string to convert.
 *   value 		: The pointer to result value.
 *
 * Return     	: The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr str_to_long(const char *string, long *value);

/*
 * ---------------------------------------------------------------------------
 * Function   	: str_to_float
 * Description	: This function converts a string into a float value using the
 *              	C command strtof.
 *
 * Param      	:
 *   string 	: The string to convert.
 *   value		: The pointer to result value.
 *
 * Return     	: The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
ThrowablePtr str_to_float(char *string, float *value);

/*
 * ---------------------------------------------------------------------------
 * Function   	: str_to_lower
 * Description	: This function converts a string into the same string with all
 *              the chars as lowercase letters using C command tolower
 *
 * Param      	:
 *   string 	:    The string to convert.
 *
 * Return     	: None
 * ---------------------------------------------------------------------------
 */
char *str_to_lower(char *string);

/*
 * ---------------------------------------------------------------------------
 * Function   	: get_error_by_errno
 * Description	: This function returns a string where there is the errorno
 *              	in a human readable way.
 *
 * Param      	:
 *   error      : The errno number.
 *
 * Return     	: None.
 * ---------------------------------------------------------------------------
 */
// TODO capire come gestire il risultato (si fa ritornare il puntatore o ne si accetta uno?)
char *get_error_by_errno(const int error);

/*
 * ---------------------------------------------------------------------------
 * Function   	: timestamp
 * Description	: This function returns a string where there is the timestamp
 *              in a human readable way.
 *
 * Param      	:
 *
 * Return     	: The timestamp in a human readable way.
 * ---------------------------------------------------------------------------
 */
char *timestamp();

/*
 * ---------------------------------------------------------------------------
 * Function   	: concat_string
 * Description	: This function put inside new_string a string results from 
 *				concatenation between string1 and string2.
 *
 * Param		: 
 *	string1		: First string
 *	string2		: Second string
 *	new_string	: Passing NULL, you will get here the string result.
 *
 * Return 		: 0 if is ok, -1 on error.
 * ---------------------------------------------------------------------------
 */
char* concat_string(const char* string1, const char* string2);

/*
 * ---------------------------------------------------------------------------
 * Function   	: append_char_to_string
 * Description	: This function append to string a new char c and \0.
 *
 * Param		: 
 *	string1		: String
 *	string2		: Character to append.
 *
 * Return 		: New string, or same string as input on error.
 * ---------------------------------------------------------------------------
 */
char* append_char_to_string(const char* string, const char c);

#endif //HELPER_H










