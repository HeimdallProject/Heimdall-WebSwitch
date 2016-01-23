#include "../include/heimdall_config.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Config
 * ---------------------------------------------------------------------------
 */
void *singleton_config = NULL;

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
        
    if (strcmp(key, "algorithm_selection") == 0) {
        if (asprintf(&config->algorithm_selection, "%s", value) == -1)
            return -1;

    }else if (strcmp(key, "pre_fork") == 0) {
        if (asprintf(&config->pre_fork, "%s", value) == -1)
            return -1;
    
    }else if (strcmp(key, "print_enable") == 0) {
        if (asprintf(&config->print_enable, "%s", value) == -1)
            return -1;

    }else if (strcmp(key, "log_level") == 0) {
        if (asprintf(&config->log_level, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "write_enable") == 0) {
        if (asprintf(&config->write_enable, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "log_file_req") == 0) {
        if (asprintf(&config->log_file_req, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "log_file_resp") == 0) {
        if (asprintf(&config->log_file_resp, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "timeout_worker") == 0) {
        if (asprintf(&config->timeout_worker, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "killer_time") == 0) {
        if (asprintf(&config->killer_time, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "update_time") == 0) {
        if (asprintf(&config->update_time, "%s", value) == -1)
            return -1;
        
    }else if (strcmp(key, "server_config") == 0) {
        if (asprintf(&config->server_config, "%s", value) == -1)
            return -1;
    
    }else if (strcmp(key, "server_main_port") == 0) {
        if (asprintf(&config->server_main_port, "%s", value) == -1)
            return -1;

    }else if (strcmp(key, "backlog") == 0) {
        if (asprintf(&config->backlog, "%s", value) == -1)
            return -1;

    }else if (strcmp(key, "max_fd") == 0) {
        if (asprintf(&config->max_fd, "%s", value) == -1)
            return -1;

    } else
        return -1;  /* unknown key, error */

    return 0;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : _get
 * Description  : Used for get key and value from a line.
 *
 * Param        :
 *   array[]    : line string.
 *   from       : from value start.
 *   to         : escape character where function stop.
 *
 * Return       : string.
 * ---------------------------------------------------------------------------
 */
char *_get(char array[], int from, char escape){

    int from_cpy = from;
    int total = 0;

    while (1) {
        if (array[from_cpy] == escape)
            break;
        ++from_cpy;
        ++total;
    }

    total++; // add \0 space

    char *subset = malloc(sizeof(char) * total);    
    if (subset == NULL) {
        fprintf(stderr, "Error in _get config_parser. \n");
        return NULL;
    }

    int j;
    for(j = 0; j < total - 1; ++j, ++from) {
        subset[j] = array[from];
    }

    subset[total-1] = '\0';

    return subset;
}

/*
 *  See .h for more information.
 */
int init_config(const char *path, int config_handler(char *key, char *value, void *p_config), void *ptr_config) {

    singleton_config = ptr_config;

    // open file
    FILE *config_file = fopen(path, "r");
    if (config_file == NULL) {
        fprintf(stderr, "Error while trying to open config file. \n");
        return -1;
    }

    // while each line
    char string[MAX_LENGTH];

    while(fgets(string, MAX_LENGTH, config_file)) {

        // skip, line comment or empty line
        if (string[0] == '#' || string[0] == '\n')
            continue;
        
        char *key = _get(string, 0, ESCAPE_CHARACTER);
        if (key == NULL) {
            fprintf(stderr, "Error in init_config config_parser. \n");
            return -1;
        }

        char *value = _get(string, strlen(key)+1, '\n');
        if (value == NULL) {
            fprintf(stderr, "Error in init_config config_parser. \n");
            return -1;
        }

        printf("%s (%d) + %s (%d) \n", key, (int)strlen(key), value, (int)strlen(value));

        if(config_handler(key, value, ptr_config) == -1) {
            fprintf(stderr, "Error config parser, no key '%s' found in Config. \n", key);
            return -1;
        }

        free(value);
        free(key);
    }

    fclose(path);

    return 0;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : new_log
 * Description  : Alloc and initialize object Log.
 *
 * Return       : Pointer to object Log, NULL on error.
 * ---------------------------------------------------------------------------
 */
ConfigPtr new_config() {

    ConfigPtr config  = malloc(sizeof(Config));
    if (config == NULL) {
        fprintf(stderr, "%s Error in malloc(sizeof(Config))\n", TAG_CONFIG);
        return NULL;
    }

    if(init_config(CONFIGFILE, &config_handler, config) == -1){
        fprintf(stderr, "%s Error in init_config\n", TAG_CONFIG);
        return NULL;
    }

    return config;
}

/*
 *  See .h for more information.
 */
void *get_config(){

    if(singleton_config == NULL){
        singleton_config = new_config();
    }

    return singleton_config;
}