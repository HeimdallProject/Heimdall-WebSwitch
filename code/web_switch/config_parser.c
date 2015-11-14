//
//============================================================================
// Name             : config_parser.c
// Author           : Andrea Cerra
// Version          : 0.2
// Data Created     : 30/05/2015
// Last modified    : 14/11/2015
// Description      : This file contains all the stuffs useful in order to create an object Config from a config file
// ===========================================================================
//

#include "config_parser.h"
#include <stdlib.h>

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Config
 * ---------------------------------------------------------------------------
 */
void *singleton_config = NULL;

/*
 * ---------------------------------------------------------------------------
 * Function     : sub_string
 * Description  : Used for get a substring from string.
 *
 * Param        :
 *   array[]    :
 *   from       :
 *   to         :
 *
 * Return       : new string.
 * ---------------------------------------------------------------------------
 */
char *get_value(char array[], int value_start_from, char escape){

    int count = 0;
    while (1) {
        if (array[count] == escape)
            break;
        ++count;
    }

    char *subset = malloc(sizeof(char*));

    int j = 0;
    for(j = 0; j < count; ++j, ++value_start_from){
        subset[j] = array[value_start_from];
    }

    return subset;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : new_config
 * Description  : Parse config file and call callback function for return values.
 *
 * Param        :
 *   path       : Path to file be parsed
 *
 * Return       :
 * ---------------------------------------------------------------------------
 */
int init_config(const char *path, void config_handler(char *, char *, void *), void *ptr_config) {

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

        char *value = get_value(string, i, '\0');
        char *key = get_value(string, 0, ESCAPE_CHARACTER);

        config_handler(key, value, ptr_config);

    }

    return 0;
}

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
void *get_config(){

    return singleton_config;
}