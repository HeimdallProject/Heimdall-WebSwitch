//
//============================================================================
// Name             : config_parser.h
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This file contains some configuration for parsing a config file
// ===========================================================================
//

#include <stdio.h>

/*
 * ---------------------------------------------------------------------------
 * Max length of line in config file
 * ---------------------------------------------------------------------------
 */
#define MAX_LENGTH 200

/*
 * ---------------------------------------------------------------------------
 * Path to config file to be parsed
 * ---------------------------------------------------------------------------
 */
#define CONFIG_FILE "/Users/Andrea/MEGAsync/Sviluppo/Projects_C/Config_Parser/config.conf"

/*
 * ---------------------------------------------------------------------------
 * Escape character used in config file, 'space' by default
 * ---------------------------------------------------------------------------
 */
#define ESCAPE_CHARACTER ' '

/*
 * ---------------------------------------------------------------------------
 * Structure    : typedef struct config
 * Description  : This struct collect all attributes for the Config object.
 *
 * Data         : Variables config file.
 * ---------------------------------------------------------------------------
 */
typedef struct config{
    char *variabile_a;
} Config;

/*
 * ---------------------------------------------------------------------------
 * Function     : get_config
 * Description  : Return instance of Config singleton.
 *
 * Param        :
 *
 * Return       : Config typedef.
 * ---------------------------------------------------------------------------
 */
Config *get_config();