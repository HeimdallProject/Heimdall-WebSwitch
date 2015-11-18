//
//============================================================================
// Name             : config_parser.c
// Author           : Andrea Cerra
// Version          : 0.3
// Data Created     : 30/05/2015
// Last modified    : 16/11/2015
// Description      : Simple C Config Parser
// ===========================================================================
//

#include "config_parser.h"

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Config
 * ---------------------------------------------------------------------------
 */
void *singleton_config = NULL;

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
 *  See .h for more information.
 */
void *get_config(){

    return singleton_config;
}