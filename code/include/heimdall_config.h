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

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAG_CONFIG "CONFIG_PARSER"

/*
 * ---------------------------------------------------------------------------
 * Example usage.
 * ---------------------------------------------------------------------------
 *
 * #include "../include/heimdall_config.h"
 *
 *  ConfigPtr config = get_config();
 *  printf("handling_mode: %s",config->handling_mode);
 *  printf("max_worker: %s",config->max_worker);
 *  printf("pre_fork: %s",config->pre_fork);
 *
 */

/*
 * ---------------------------------------------------------------------------
 * Relative position to config file
 * ---------------------------------------------------------------------------
 */
#define CONFIGFILE "../code/config/heimdall_config.conf"

/*
 * ---------------------------------------------------------------------------
 * Structure        : Config struct
 * Description      : This struct collect all config value from config file.
 * ---------------------------------------------------------------------------
 */
typedef struct config{
    char *handling_mode;
    char *max_worker;
    char *max_thread_for_worker;
    char *algorithm_selection;
    char *pre_fork;
    char *log_level;
    char *write_enable;
    char *print_enable;
    char *log_file;
    char *timeout_worker;
    char *killer_time;
    char *server_config;
    char *timeout_request;
} Config, *ConfigPtr;

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
 * Function     : init_config
 * Description  : Parse config file and call callback function for return values.
 *
 * Param            :
 *   path           : Path to file be parsed.
 *   config_handler : Callback function, return to main key, value and config reference.
 *   ptr_config     : Pointer to struct.
 *
 * Return       :
 * ---------------------------------------------------------------------------
 */
int init_config(const char *path, int config_handler(char *key, char *value, void *p_config), void *ptr_config);

/*
 * ---------------------------------------------------------------------------
 * Function     : get_config
 * Description  : Return singleton config pointer.
 *
 * Param        :
 *
 * Return       : Pointer to object Config.
 * ---------------------------------------------------------------------------
 */
void *get_config();

#endif //CONFIG_PARSER_H