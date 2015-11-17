#include <string.h>
#include "stdlib.h"
#include "stdio.h"

#include "utils/log.h"
#include "web_switch/thread_pool.h"
#include "web_switch/worker_obj.h"
#include "web_switch/config_parser.h"
#include "web_switch/worker_pool.h"


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

    printf("Key %s %d\n", key,strcmp(key, "handling_mode"));

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

    /* Init Log */
    Log log = *get_log();
    log.d(TAG_MAIN, "Start main programm");
    log.d(TAG_MAIN, "Start Log");

    /* Init Config */

    /*Config *config  = malloc(sizeof(Config));
    if (config == NULL) {
        log.d(TAG_MAIN, "Error in malloc(sizeof(Config))");
        exit(EXIT_FAILURE);
    }

    if(init_config(CONFIGFILE, &config_handler, config) == -1){
        log.d(TAG_MAIN, "Error in init_config");
        exit(EXIT_FAILURE);
    }

    log.d(TAG_MAIN, "Config started");*/

    /*printf("handling_mode: %s\n",config->handling_mode);
    printf("max_worker: %s\n",config->max_worker);
    printf("max_thread_for_worker: %s\n",config->max_thread_for_worker);
    printf("algorithm_selection: %s\n",config->algorithm_selection);
    printf("pre_fork: %s\n",config->pre_fork);
    printf("log_level: %s\n",config->log_level);
    printf("write_enable: %s\n",config->write_enable);
    printf("print_enable: %s\n",config->print_enable);
    printf("log_file: %s\n",config->log_file);
    printf("timeout_worker: %s\n",config->timeout_worker);
    printf("killer_time: %s\n",config->killer_time);
    printf("server_config: %s\n",config->server_config);
    printf("timeout_request: %s\n",config->timeout_request);*/

    /*for further access in config please use:
     Config *config1 = get_config(); */

    // Creazione del thread Pool
    /*ThreadPool *th_pool = init_thread_pool();
    if (th_pool == NULL){
        log.d(TAG_MAIN, "Error in init_thread_pool()");
        exit(EXIT_FAILURE);
    }

    Worker *wrk = th_pool->get_worker();
    printf("Worker: %p\n", wrk->thread_identifier);*/

    WorkerPoolPtr ptr = init_worker_pool();
    printf("%p\n", ptr);    

    ptr->add_worker(NULL);
    ptr->search_worker(NULL);
    ptr->delete_worker(NULL);
    
    exit(EXIT_SUCCESS);
}