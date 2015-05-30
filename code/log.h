//
//============================================================================
// Name             : log.h
// Author           : Andrea Cerra e Claudio Pastorini
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This header file contains all the stuffs useful in order to print / write log file.
// ===========================================================================
//

#include "throwable.h"
#include "apache_status.h"

#define DEBUG_LEVEL   2  // Macro for debug message level
#define INFO_LEVEL    3  // Macro for info message level
#define ERROR_LEVEL   4  // Macro for error message level

//TODO da mettere nel config
#define PRINT_LEVEL 2

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct log
 * Description  : This struct collect all functions pointers for logging.
 *
 * Functions:
 *  d                               : Pointer to debug function.
 *  i                               : Pointer to info function.
 *  e                               : Pointer to error function.
 *  print_throwable                 : Pointer to print throwable function.
 *  print_apache_server_status:     : Pointer to print apache server status function.
 * ---------------------------------------------------------------------------
 */
typedef struct log {
    void (*d)(char* tag, char *msg);
    void (*i)(char* tag, char *msg);
    void (*e)(char* tag, char *msg);
    void (*print_throwable)(Throwable *throwable);
    void (*print_apache_server_status)(ApacheServerStatus *apacheServerStatus);
} Log;

/*
 * ---------------------------------------------------------------------------
 * Function     : get_log
 * Description  : Return instance of log singleton.
 *
 * Param        :
 *
 * Return       : Log typedef.
 * ---------------------------------------------------------------------------
 */
Log *get_log();