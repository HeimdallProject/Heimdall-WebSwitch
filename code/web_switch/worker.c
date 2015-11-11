//
//============================================================================
// Name             : worker.c
// Author           : Alessio Moretti e Claudio Pastorini
// Version          : 0.2
// Data Created     : 11/11/2015
// Last modified    : 11/11/2015
// Description      : This is the webswitch single Worker thread body
// ===========================================================================
//

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "worker.h"

int start_worker() {
    time_t timestamp;

    // initializing write thread
    // ** do something to start the thread**

    // initializing watchdog
    // ** do something to start the thread **

    // initialilizing the FIFO data structure to manage a
    // node of the current request handled (pipeline-robust approach)
    // ** do something to allocate the FIFO **

    // starting the main routine cycling in a read/execute loop
    while(TRUE) {
        // ** reading and passing params to HTTPRequest setting the timestamp **/
        timestamp = time(NULL);

    }

    // successfull status
    return STATUS_OK;
}