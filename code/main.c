#include <string.h>
#include "stdlib.h"
#include "stdio.h"
#include <unistd.h>

#include "utils/log.h"
#include "web_switch/thread_pool.h"
#include "web_switch/worker_obj.h"
#include "web_switch/config_parser.h"


/*
 * ---------------------------------------------------------------------------
 *
 * Program macro
 *
 * ---------------------------------------------------------------------------
 */

#define CONFIGFILE "../heimdall_config.conf"
#define TAG_MAIN "MAIN"

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
} Config;

/*
 * ---------------------------------------------------------------------------
 * Function     : config_handler
 * Description  : Callback function, see config_parser.c for more information.
 *
 * Return       : 0 if ok, -1 if error.
 * ---------------------------------------------------------------------------
 */
int config_handler(char *key, char *value, void *p_config) {

    Config* config = (Config *)p_config;

    if (strcmp(key, "handling_mode") == 0)
        config->handling_mode = value;
    else if (strcmp(key, "max_worker") == 0)
        config->max_worker = value;
    else if (strcmp(key, "max_thread_for_worker") == 0)
        config->max_thread_for_worker = value;
    else if (strcmp(key, "algorithm_selection") == 0)
        config->algorithm_selection = value;
    else if (strcmp(key, "pre_fork") == 0)
        config->pre_fork = value;
    else if (strcmp(key, "log_level") == 0)
        config->log_level = value;
    else if (strcmp(key, "write_enable") == 0)
        config->write_enable = value;
    else if (strcmp(key, "print_enable") == 0)
        config->print_enable = value;
    else if (strcmp(key, "log_file") == 0)
        config->log_file = value;
    else if (strcmp(key, "timeout_worker") == 0)
        config->timeout_worker = value;
    else if (strcmp(key, "killer_time") == 0)
        config->killer_time = value;
    else if (strcmp(key, "server_config") == 0)
        config->server_config = value;
    else if (strcmp(key, "timeout_request") == 0)
        config->timeout_request = value;
    else
        return -1;  /* unknown key, error */

    return 0;
}

/*
 * ---------------------------------------------------------------------------
 *
 * MAIN PROGRAM
 *
 * ---------------------------------------------------------------------------
 */
int main() {

    /* Init Config */
    Config *config  = malloc(sizeof(Config));
    if (config == NULL) {
        fprintf(stderr, "%s Error in malloc(sizeof(Config))\n", TAG_MAIN);
        exit(EXIT_FAILURE);
    }

    if(init_config(CONFIGFILE, &config_handler, config) == -1){
        fprintf(stderr, "%s Error ininit_config\n", TAG_MAIN);
        exit(EXIT_FAILURE);
    }

    /*for further access in config please use:
     Config *config1 = get_config(); */

    /*printf("handling_mode: %s",config->handling_mode);
    printf("max_worker: %s",config->max_worker);
    printf("max_thread_for_worker: %s",config->max_thread_for_worker);
    printf("algorithm_selection: %s",config->algorithm_selection);
    printf("pre_fork: %s",config->pre_fork);
    printf("log_level: %s",config->log_level);
    printf("write_enable: %s",config->write_enable);
    printf("print_enable: %s",config->print_enable);
    printf("log_file: %s",config->log_file);
    printf("timeout_worker: %s",config->timeout_worker);
    printf("killer_time: %s",config->killer_time);
    printf("server_config: %s",config->server_config);
    printf("timeout_request: %s",config->timeout_request);*/

    /* Init Log */
    //LogPtr log = get_log();

    //log->d(TAG_MAIN, "Start main programm");
    //log->d(TAG_MAIN, "Prova1 log con intero %d e caratter %c", 100, 'Y');

    //log->i(TAG_MAIN, "Prova2 log con intero %d e caratter %c", 100, 'Y');
    //log->e(TAG_MAIN, "Prova3 log con intero %d e caratter %c", 100, 'Y');
    //log->d(TAG_MAIN, "Start Log");

    //log.d(TAG_MAIN, "Config started");

    // Creazione del thread Pool
    /*ThreadPool *th_pool = init_thread_pool();
    if (th_pool == NULL){
        log->d(TAG_MAIN, "Error in init_thread_pool()");
        exit(EXIT_FAILURE);
    }*/

    /*WorkerPtr wrk = th_pool->get_worker();
    printf("Worker: %p\n", wrk->thread_identifier);

    int i = 0;
    for (i = 0; i < 10000; ++i){
        sleep(1);
        printf(".");
    }*/
    
    exit(EXIT_SUCCESS);
}