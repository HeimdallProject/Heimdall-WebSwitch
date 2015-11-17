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
#ifndef LOG_H
#define LOG_H
#define _GNU_SOURCE

#include "throwable.h"
#include "../web_switch/apache_status.h"
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"
#include <stdarg.h>
#include <string.h>

#define DEBUG_LEVEL   2  // Macro for debug message level
#define INFO_LEVEL    3  // Macro for info message level
#define ERROR_LEVEL   4  // Macro for error message level

//TODO da mettere nel config
#define PRINT_LEVEL 2

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct log
 * Description      : This struct collect all functions pointers for logging.
 *
 * Functions:
 *  d       : Pointer to debug function.
 *  i       : Pointer to info function.
 *  e       : Pointer to error function.
 *  t       : Pointer to print throwable function.
 * ---------------------------------------------------------------------------
 */
typedef struct log {
    int (*d)(const char* tag, const char *format, ...);
    int (*i)(const char* tag, const char *format, ...);
    int (*e)(const char* tag, const char *format, ...);
    void (*t)(Throwable *throwable);
} Log, *LogPtr;

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

#endif