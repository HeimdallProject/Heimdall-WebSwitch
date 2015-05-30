//
//============================================================================
// Name             : config_parser.c
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 30/05/2015
// Last modified    : 30/05/2015
// Description      : This file contains all the stuffs useful in order to create an object Config from a config file
// ===========================================================================
//

#include "config_parser.h"
#include <string.h>
#include "errno.h"
#include <stdlib.h>

/*
 * ---------------------------------------------------------------------------
 * Description  : Global variable, singleton instance of Config
 * ---------------------------------------------------------------------------
 */
Config *singleton_config = NULL;

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
char *sub_string(char array[], int from, __unused int to){

    int count = 0;
    while (1){
        if (array[count] == '\0')
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
 * ---------------------------------------------------------------------------
 * Function     : parse_config_file
 * Description  : Used for parsing a config file.
 *
 * Param        :
 *   path       : Path to file be parsed
 *   config     : Pointer to object Config where this function save the results.
 *
 * Return       : void.
 * ---------------------------------------------------------------------------
 */
void parse_config_file(const char *path, Config *config){

    errno = 0;
    char **base_address = (char**)config;
    int offset = 0;

    // open file
    FILE *config_file = fopen(path, "r");
    if (config_file == NULL) {
        fprintf(stderr, "Error while trying to open config file %s - %s\n.", path, strerror(errno));
        exit(EXIT_FAILURE);
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

        ++i;
        char **value = base_address+offset;
        *value = sub_string(string, i, MAX_LENGTH);

        offset += 1;
    }
}

/*
 * ---------------------------------------------------------------------------
 * Function     : new_config
 * Description  : Alloc and initialize object Config, by default all value of this object are set to NULL.
 *
 * Param        :
 *
 * Return       : Pointer to objec Log.
 * ---------------------------------------------------------------------------
 */
Config *new_config () {

    // inizialize object
    Config *config = malloc (sizeof(Config));
    if (config == NULL){
        fprintf(stderr, "Error with malloc in Config initialization \n");
        exit(EXIT_FAILURE);
    }

    int count = sizeof(Config)/ sizeof(char*);
    int offset = 0;

    // set all value of struct to NULL
    while(count > 0){

        char **base_address = (char**)config;
        char **value = base_address+offset;
        *value = NULL;

        count -= 1;
        offset += 1;
    }

    // parse file and initialize config struct
    parse_config_file(CONFIG_FILE, config);

    return config;
}

/*
 * ---------------------------------------------------------------------------
 * Function     : get_config
 * Description  : Return singleton instance of Config object, if this instance is not already created
 *                this function call automatically new_config() for instantiate the object itself.
 *
 * Param        :
 *
 * Return       : Pointer to object Config.
 * ---------------------------------------------------------------------------
 */
Config *get_config(){

    if (singleton_config == NULL){
        singleton_config = new_config();
    }

    // return singleton_config
    return singleton_config;
}

/*
 * ---------------------------------------------------------------------------
 *  Main function, for test and example usage.
 * ---------------------------------------------------------------------------
 */
int main() {

    Config *config = get_config();
    printf("%s\n",config->variabile_a);

    return 0;
}