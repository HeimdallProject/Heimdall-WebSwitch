//
//============================================================================
// Name             : thread_pool.c
// Author           : Andrea Cerra
// Version          : 0.1
// Data Created     : 16/11/2015
// Last modified    : 16/11/2015
// Description      : This header file contains infomation about worker object.
// ===========================================================================
//

#include "worker_obj.h"

/*
 *  See .h for more information.
 */
Worker *new_worker() {

    Worker *wrk = malloc(sizeof(Worker));
    if (wrk == NULL) {
        fprintf(stderr, "Memory allocation error in new_log.\n");
        exit(EXIT_FAILURE);
    }

    // Set attribute
    wrk->thread_identifier = NULL;
    wrk->process_identifier = NULL;

    return wrk;
}
