//
//============================================================================
// Name             : worker.h
// Author           : Alessio Moretti e Claudio Pastorini
// Version          : 0.2
// Data Created     : 11/11/2015
// Last modified    : 11/11/2015
// Description      : This is the webswitch single Worker thread body
// ===========================================================================
//
#ifndef WEBSWITCH_WORKER_H
#define WEBSWITCH_WORKER_H

#include "../utils/helper.h"

/*
 * ---------------------------------------------------------------------------
 * Function   : start_worker
 * Description: This function runs the main loop into which the worker operates
 *              managing the connection between the client and the remote machine
 *
 * Param      :
 * Return     : STATUS_OK on successfull operations status, STATUS_ERROR otherwise
 * ---------------------------------------------------------------------------
 */
int start_worker();

#endif //WEBSWITCH_WORKER_H
