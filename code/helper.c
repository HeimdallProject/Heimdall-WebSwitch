//
//============================================================================
// Name       : utils.h
// Author     : Claudio Pastorini
// Version    : 0.2
// Description: Header file with some useful macro and functions.
// ===========================================================================
//

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "helper.h"
#include "time.h"


Throwable *str_to_int(char *string, int *value) {

    char *pointer;

    errno = 0;
    *value = (int) strtol(string, &pointer, 0);
    if (errno != 0 || *pointer != '\0') (*get_throwable()).create(STATUS_ERROR, "", "str_to_int");

    return (*get_throwable()).create(STATUS_OK, "", "str_to_int");
}

Throwable *str_to_float(char *string, float *value) {

    char *pointer;

    errno = 0;
    *value = strtof(string, &pointer);
    if (errno != 0 || *pointer != '\0') (*get_throwable()).create(STATUS_ERROR, "", "str_to_float");

    return (*get_throwable()).create(STATUS_OK, "", "str_to_float");
}

char *str_to_lower(char *string) {
    int i;
    for (i = 0; string[i]; i++) {
        string[i] = (char) tolower(string[i]);
    }

    return string;
}

char *timestamp() {

    time_t current_time;
    current_time = time(NULL);

    return asctime(localtime(&current_time));
}

//TODO fare funzione che rimuove (che fa il replace) \n, da usare in timestamp