//
// Created by Odysseus on 18/11/15.
//

#ifndef WEBSWITCH_CONFIG_H
#define WEBSWITCH_CONFIG_H

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

#endif //WEBSWITCH_CONFIG_H
