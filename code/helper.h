//
//============================================================================
// Name       : utils.h
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: Header file with some useful macro and functions.
// ===========================================================================
//

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
int str_to_int(char *string);

/*
 * ---------------------------------------------------------------------------
 * Function   : str_to_float
 * Description: This function converts a string into a float value using the
 *              C command strtof.
 *
 * Param      :
 *   string:    The string to convert.
 *
 * Return     : The converted value or STATUS_ERROR in case of error.
 * ---------------------------------------------------------------------------
 */
float str_to_float(char *string);


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