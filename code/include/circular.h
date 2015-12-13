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
#include "../include/server_pool.h"

#define TAG_CIRCULAR "CIRCULAR"

#define BUFFER_PROGRESS_OK      0
#define BUFFER_PROGRESS_STOP    -1

/*
 * this is only an example structure to define a circular buffer over a set of
 * finite web servers.
 */
typedef struct server {
    char *address;
    char *ip;
    int  port;
    int  weight;
} Server, *ServerPtr;

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

    pthread_mutex_t mutex;

    ThrowablePtr (*allocate_buffer)(struct circular_buffer *circular, Server **servers, int len);
    ThrowablePtr (*acquire)(struct circular_buffer *circular);
    ThrowablePtr (*release)(struct circular_buffer *circular);
    void         (*progress)(struct circular_buffer *circular);
    void         (*destroy_buffer)(struct circular_buffer *circular);
} Circular, *CircularPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : allocate_buffer
 * Description: This function is used to allocate a finite buffer of finite
 *              remote machines
 *
 * Param      :
 *   CircularPtr
 *   struct server array: to allocate the buffer
 *   integer len:         the length of the server array
 *
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr allocate_buffer(CircularPtr circular, Server **servers, int len);

/*
 * ---------------------------------------------------------------------------
 * Function   : acquire_circular
 * Description: This function is used to acquire the lock primitive associated
 *              to the singleton
 * Param      : CircularPtr
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr acquire_circular(CircularPtr circular);

/*
 * ---------------------------------------------------------------------------
 * Function   : release_circular
 * Description: This function is used to release the lock primitive associated
 *              to the singleton
 * Param      : CircularPtr
 * Return     : ThrowablePtr
 * ---------------------------------------------------------------------------
 */
ThrowablePtr release_circular(CircularPtr circular);

/*
 * ---------------------------------------------------------------------------
 * Function   : progress
 * Description: This function is used to move along the circular buffer
 *
 * Param      : CircularPtr
 * Return     :
 * ---------------------------------------------------------------------------
 */
void progress(CircularPtr circular);


/*
 * ---------------------------------------------------------------------------
 * Function   : destroy_buffer
 * Description: This function is used to deallocate the buffer
 *
 * Param      : CircularPtr
 * Return     :
 * ---------------------------------------------------------------------------
 */
void destroy_buffer(CircularPtr circular);


/*
 * ---------------------------------------------------------------------------
 * Function   : new_circular
 * Description: This function is used to initialize the Circular struct
 *
 * Param      :
 *
 * Return     : CircularPtr
 * ---------------------------------------------------------------------------
 */
CircularPtr new_circular();



#endif //WEBSWITCH_CIRCULAR_H