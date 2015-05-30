//
//============================================================================
// Name       : utils.h
// Author     : Claudio Pastorini
// Version    : 0.1
// Description: Header file with some useful macro and functions.
// ===========================================================================
//

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "helper.h"
#include "time.h"


int str_to_int(char *string) {

    char *pointer;
    int value;

    errno = 0;
    value = (int) strtol(string, &pointer, 0);
    if (errno != 0 || *pointer != '\0') {
        fprintf(stderr, "Invalid number\n");
        return STATUS_ERROR;
    }

    return value;
}

float str_to_float(char *string) {

    char *pointer;
    float value;

    errno = 0;
    value = strtof(string, &pointer);
    if (errno != 0 || *pointer != '\0') {
        fprintf(stderr, "Invalid number\n");
        return STATUS_ERROR;
    }

    return value;
}

char *str_to_lower(char *string) {
    int i;
    for (i = 0; string[i]; i++) {
        string[i] = (char) tolower(string[i]);
    }

    return string;
}

char *timestamp() {

    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    return asctime(localtime(&ltime));
}

//TODO fare funzione che rimuove \n, da usare in timestamp