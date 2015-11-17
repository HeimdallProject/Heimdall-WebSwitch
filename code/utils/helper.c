//
//============================================================================
// Name             : utils.h
// Author           : Claudio Pastorini
// Version          : 0.2
// Data Created     : 12/05/2015
// Last modified    : 01/06/2015
// Description      : Header file with some useful macro and functions.
// ===========================================================================
//

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "time.h"
#include "log.h"
#include "helper.h"

/*
 *  See .h for more information.
 */
Throwable *str_to_int(const char *string, int *value) {
    char *pointer;

    errno = 0;
    *value = (int) strtol(string, &pointer, 0);

    if ((errno == ERANGE) || (errno != 0 && *value == 0)) 
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "str_to_int");
    if (pointer == string) 
        return (*get_throwable()).create(STATUS_ERROR, "No digits were found", "str_to_int");
    if (*pointer != '\0') 
        return (*get_throwable()).create(STATUS_ERROR, "There is some characters", "str_to_int");

    return (*get_throwable()).create(STATUS_OK, NULL, "str_to_int");
}

/*
 *  See .h for more information.
 */
Throwable *str_to_float(char *string, float *value) {
    char *pointer;

    errno = 0;
    *value = strtof(string, &pointer);
    if ((errno == ERANGE && (*value == LONG_MAX || *value == LONG_MIN)) || (errno != 0 && *value == 0)) 
        return (*get_throwable()).create(STATUS_ERROR, get_error_by_errno(errno), "str_to_float");
    if (pointer == string) 
        return (*get_throwable()).create(STATUS_ERROR, "No digits were found", "str_to_float");
    if (*pointer != '\0') 
        return (*get_throwable()).create(STATUS_ERROR, "There is some characters", "str_to_float");

    return (*get_throwable()).create(STATUS_OK, NULL, "str_to_float");
}

/*
 *  See .h for more information.
 */
char *str_to_lower(char *string) {
    int i;
    for (i = 0; string[i]; i++) {
        string[i] = (char) tolower(string[i]);
    }

    return string;
}

/*
 *  See .h for more information.
 */
char *get_error_by_errno(const int error) {
    char error1[250];
    snprintf(error1, 250, "%d", error);
    (*get_log()).d(TAG_HELPER, error1);

    char *message;
    size_t size;

    size = 128;
    message = malloc(size);
    if (message == NULL)
        return NULL;

    while (strerror_r(error, message, size) == -1) {
        size *= 2;
        message = realloc(message, size);
        if (message == NULL)
            return NULL;
    }
    return message;
}

/*
 *  See .h for more information.
 */
char *timestamp() {
    
    // Get current time
    time_t current_time;
    current_time = time(NULL);
    
    // Get a human readable way
    char *timestamp = asctime(localtime(&current_time));

    int count = 0;
    while (1) {
        if (timestamp[count] == '\n'){
            timestamp[count] = '\0';
            break;
        }
        ++count;
    }
    
    return timestamp;
}

/*
 *  See .h for more information.
 */
char* concat_string(const char* string1, const char* string2){

    int len_str1 = strlen(string1);
    int len_str2 = strlen(string2);

    char *new_string = (char*) malloc(len_str1+len_str2);
    if (new_string == NULL) {
        return NULL;
    }

    char *to = new_string;
    to = stpcpy (to, string1);
    to = stpcpy (to, string2);
    
    return new_string;
}

/*
 *  See .h for more information.
 */
char* append_char_to_string(const char* string, const char c){

    size_t len = strlen(string);

    char* ret = (char*) malloc(len+2);
    if (ret == NULL) {
        return NULL;
    }

    strcpy(ret, string);    
    ret[len] = c;
    ret[len+1] = '\0';

    return ret;
}








