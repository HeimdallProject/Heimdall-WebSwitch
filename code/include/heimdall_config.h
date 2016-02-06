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
    char *sockets_path;
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