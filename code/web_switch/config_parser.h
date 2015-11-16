//
//============================================================================
// Name             : config_parser.c
// Author           : Andrea Cerra
// Version          : 0.2
// Data Created     : 30/05/2015
// Last modified    : 14/11/2015
// Description      : This file contains all the stuffs useful in order to create an object Config from a config file
// ===========================================================================
//

#include <stdio.h>
#include <stdlib.h>

/*
 * ---------------------------------------------------------------------------
 * Max length of line in config file
 * ---------------------------------------------------------------------------
 */
#define MAX_LENGTH 200

/*
 * ---------------------------------------------------------------------------
 * Escape character used in config file, 'space' by default
 * ---------------------------------------------------------------------------
 */
#define ESCAPE_CHARACTER ' '

/*
 * ---------------------------------------------------------------------------
 * Function         : new_config
 * Description      : Return instance of Config singleton.
 *
 * Param            :
 *
 * Return           : Config typedef.
 * ---------------------------------------------------------------------------
 */
int init_config(const char *path, int config_handler(char *key, char *value, void *p_config), void *ptr_config);

/*
 * ---------------------------------------------------------------------------
 * Function     : get_config
 * Description  : Return pointer of Config singleton.
 *
 * Param        :
 *
 * Return       : void pointer.
 * ---------------------------------------------------------------------------
 */
void *get_config();