#include "../include/server_config.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Config
 * ---------------------------------------------------------------------------
 */
void *singleton_server_config = NULL;

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
char *line_get(char array[], int from, char escape){

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
        get_log()->e(TAG_SERVER_CONFIG, "Error line_get in server_config");   
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
int parse_line(ServerConfigPtr config) {

    ConfigPtr him_config = get_config();

    // open file
    FILE *server_file = fopen(him_config->server_config, "r");
    if (server_file == NULL) {
        get_log()->e(TAG_SERVER_CONFIG, "Error while trying to open config file in parse_line");
        return -1;
    }

    // while each line
    char string[MAX_LENGTH];

    int count_name = 0;
    int count_ip = 0;

    while(fgets(string, MAX_LENGTH, server_file)) {

        // skip, line comment or empty line
        if (string[0] == '#' || string[0] == '\n')
            continue;
        
        char *type = line_get(string, 0, ESCAPE_CHR);
        if (type == NULL) {
            get_log()->e(TAG_SERVER_CONFIG, "Error parse_line (1) in server_config");   
            return -1;
        }

        char *value = line_get(string, strlen(type) + 1, '\n');
        if (value == NULL) {
            get_log()->e(TAG_SERVER_CONFIG, "Error parse_line (2) in server_config");   
            return -1;
        }

        get_log()->i(TAG_SERVER_CONFIG, "%s (%d) + %s (%d)", type, (int)strlen(type), value, (int)strlen(value));        

        if(strcmp(type, "IP") == 0){
            config->servers_ip[count_ip] = value;
            count_ip++;
        }

        if(strcmp(type, "Name") == 0){
            config->servers_names[count_name] = value;
            count_name++;
        }
    }

    fclose(server_file);

    return 0;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : count_server
 * Description  : Count how many lines start with IP :D
 *
 * Return       : Total server in config file
 * ---------------------------------------------------------------------------
 */
int count_server() {

    ConfigPtr him_config = get_config();

    // open file
    FILE *server_file = fopen(him_config->server_config, "r");
    if (server_file == NULL) {
        get_log()->e(TAG_SERVER_CONFIG, "Error while trying to open config file in count_server");
        return -1;
    }

    // while each line
    char string[3];

    int server_count = 0;

    while(fgets(string, 3, server_file)) {
        if (string[0] == 'I' && string[1] == 'P')
            server_count++;
    }

    fclose(server_file);

    return server_count;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : new_server_config
 * Description  : Alloc and initialize object ServerConfigPtr.
 *
 * Return       : Pointer to object ServerConfigPtr, NULL on error.
 * ---------------------------------------------------------------------------
 */
ServerConfigPtr new_server_config() {

    ServerConfigPtr config = malloc(sizeof(ServerConfig));
    if (config == NULL) {
        get_log()->e(TAG_SERVER_CONFIG, "Error in malloc (1) new_server_config");
        return NULL;
    }

    int total = count_server();
    config->total_server = total;

    get_log()->i(TAG_SERVER_CONFIG, "Server total: %d", config->total_server);

    config->servers_names = malloc(sizeof(char*) * config->total_server);
    if (config->servers_names == NULL) {
        get_log()->e(TAG_SERVER_CONFIG, "Error in malloc (2) new_server_config");
        return NULL;
    }
    
    config->servers_ip = malloc(sizeof(char*) * config->total_server);
    if (config->servers_ip == NULL) {
        get_log()->e(TAG_SERVER_CONFIG, "Error in malloc (3) new_server_config");
        return NULL;
    }

    if (parse_line(config) != 0) {
        get_log()->e(TAG_SERVER_CONFIG, "Error in parse_line new_server_config");
        return NULL;
    }

    return config;
}

/*
 *  See .h for more information.
 */
void *get_server_config(){

    if(singleton_server_config == NULL){
        singleton_server_config = new_server_config();
    }

    return singleton_server_config;
}