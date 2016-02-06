//
//============================================================================
// Name       : heimdall_config.h
// Description: Simple C Config Parser.
// ===========================================================================
//
#ifndef CONFIG_SERVER_PARSER_H
#define CONFIG_SERVER_PARSER_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/heimdall_config.h"
#include "../include/log.h"
#include "../include/throwable.h"
#include "../include/helper.h"

#define TAG_SERVER_CONFIG "CONFIG_SERVER_PARSER"

/*
 * ---------------------------------------------------------------------------
 * Structure        : Config struct
 * Description      : This struct collect all config value from config file.
 * ---------------------------------------------------------------------------
 */
typedef struct server_config{
    char **servers_names;
    char **servers_ip;
    int total_server;
} ServerConfig, *ServerConfigPtr;

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
#define ESCAPE_CHR ':'

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
void *get_server_config();

#endif //CONFIG_SERVER_PARSER_H