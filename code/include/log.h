//
//============================================================================
// Name             : log.h
// Description      : This header file contains all the stuffs useful in order to print / write log file.
// ===========================================================================
//
#ifndef LOG_H
#define LOG_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../include/helper.h"
#include "../include/throwable.h"
#include "../include/heimdall_config.h"

#define DEBUG_LEVEL   2
#define INFO_LEVEL    3
#define ERROR_LEVEL   4

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
 *
 *  LogPtr log = get_log();
 *  log->i("TAG", "INFO %s", "Si usa come la printf");
 *  log->d("TAG", "DEBUG");
 *  log->e("TAG", "ERRORE");
 *
 */


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