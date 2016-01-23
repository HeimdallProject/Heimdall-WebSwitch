//
//============================================================================
// Name       : heimdall_config.h
// Description: Simple C Config Parser.
// ===========================================================================
//
#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H
#define _GNU_SOURCE

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
    char *algorithm_selection;
    char *pre_fork;
    char *print_enable;
    char *log_level;
    char *write_enable;
    char *log_file_req;
    char *log_file_resp;
    char *timeout_worker;
    char *killer_time;
    char *update_time;
    char *server_config;
    char *server_main_port;
    char *backlog;
    char *max_fd;
} Config, *ConfigPtr;

/*
 * ---------------------------------------------------------------------------
 * Max length of line in config file
 * ---------------------------------------------------------------------------
 */
#define MAX_LENGTH 128

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