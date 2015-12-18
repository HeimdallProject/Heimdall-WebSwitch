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

    if (strcmp(key, "handling_mode") == 0)
        config->handling_mode = value;
    else if (strcmp(key, "max_worker") == 0)
        config->max_worker = value;
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

    int count = 0;
    while (1) {
        if (array[count] == escape)
            break;
        ++count;
    }

    char *subset = malloc(sizeof(char*));

    int j = 0;
    for(j = 0; j < count; ++j, ++from){
        subset[j] = array[from];
    }

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

        // find value of variable
        int i = 0;
        while (string[i] != ESCAPE_CHARACTER)
            ++i;

        char *value = _get(string, i, '\0');
        char *key = _get(string, 0, ESCAPE_CHARACTER);

        if(config_handler(key, value, ptr_config) == -1){
            fprintf(stderr, "Error config parser, no key '%s' found in Config. \n", key);
            return -1;
        }

    }

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