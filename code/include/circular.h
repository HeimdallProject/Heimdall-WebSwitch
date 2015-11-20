//
//============================================================================
// Name       : circular.h
// Description: This file contains an implementation of the circular buffer
//              data structure with some example structures (to simulate an
//              implementation over remote machines).
//
//              It has been used a lock to perform atomically all the admin
//              operations upon the circular buffer. It is architecturally
//              intended that progress() operation is the only one performed
//              by the user and her should handle the release op.
// ===========================================================================
//
#ifndef WEBSWITCH_CIRCULAR_H
#define WEBSWITCH_CIRCULAR_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "../include/helper.h"
#include "../include/throwable.h"

#define TAG_CIRCULAR "CIRCULAR"
#define BUFFER_PROGRESS_OK      0
#define BUFFER_PROGRESS_STOP    -1
#define SERVER_STATUS_READY     1
#define SERVER_STATUS_BROKEN    -1
#define WEIGHT_DEFAULT          1
#define WEIGHT_MAX              4

/*
 * this is only an example structure to define a circular buffer over a set of
 * finite web servers.

 */
typedef struct server {
    char *address;
    int  port;
    int  status;
    int  weight;
} Server;

/*
 * ---------------------------------------------------------------------------
 * Structure        : typedef struct circular_buffer
 * Description      : This struct helps to manage a circular buffer of fixed length
 * ---------------------------------------------------------------------------
 */
typedef struct circular_buffer {
    Server      *buffer;
    int         buffer_position;
    int         buffer_len;
    Server      *head;
    Server      *tail;

    ThrowablePtr   *(*allocate_buffer)(Server **servers, int len);
    int          (*progress)();
    void         (*destroy_buffer)();
} Circular, *CircularPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : allocate_buffer
 * Description: This function is used to allocate a finite buffer of finite
 *              remote machines
 *
 * Param      :
 *   struct server array: to allocate the buffer
 *   integer len:         the length of the server array
 *
 * Return     :
 *   ThrowablePtr pointer
 * ---------------------------------------------------------------------------
 */
ThrowablePtr allocate_buffer(Server **servers, int len);


/*
 * ---------------------------------------------------------------------------
 * Function   : progress
 * Description: This function is used to move along the circular buffer
 *
 * Param      :
 *
 * Return     :
 *   buffer progress status integer indicator
 * ---------------------------------------------------------------------------
 */
int progress(void);


/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_buffer
 * Description: This function is used to deallocate the buffer
 *
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_buffer();


/*
 * ---------------------------------------------------------------------------
 * Function   : new_circular
 * Description: This function is used to initialize the Circular singleton
 *
 * Param      :
 *
 * Return     :
 *   Circular pointer
 * ---------------------------------------------------------------------------
 */
CircularPtr new_circular(void);

/*
 * ---------------------------------------------------------------------------
 * Function   : get_circular
 * Description: This function is used to retrieve the Circular singleton
 *
 * Param      :
 *
 * Return     :
 *   Circular pointer
 * ---------------------------------------------------------------------------
 */
CircularPtr get_circular(void);

/*
 * ---------------------------------------------------------------------------
 * Function   : acquire_circular
 * Description: This function is used to acquire the lock primitive associated
 *              to the singleton
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void acquire_circular(void);

/*
 * ---------------------------------------------------------------------------
 * Function   : release_circular
 * Description: This function is used to release the lock primitive associated
 *              to the singleton
 * Param      :
 * Return     :
 * ---------------------------------------------------------------------------
 */
void release_circular(void);

#endif //WEBSWITCH_CIRCULAR_H