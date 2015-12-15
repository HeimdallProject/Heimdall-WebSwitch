//
//============================================================================
// Name       : chunk.h
// Description: This header file contains all the stuffs useful in order to
//              create a chunk.
// ===========================================================================
//
#ifndef WEBSWITCH_CHUNK_H
#define WEBSWITCH_CHUNK_H

#include <pthread.h>
#include <stdlib.h>

#include "../include/log.h"
#include "../include/macro.h"

#define TAG_CHUNK "CHUNK"
#define MAX_CHUNK_SIZE 4096

/*
 * ---------------------------------------------------------------------------
 * Structure  : Chunk
 * Description: This struct allows to create a linked request node.
 *
 * Data:
 *  mutex           : The mutex to use in order to regulate the HTTP exchange.
 *  // TODO
 */
typedef struct Chunk {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int wrote;
    int dimen;
    char *data;
} Chunk, *ChunkPtr;

/*
 * ---------------------------------------------------------------------------
 * Function   : new_chunk
 * Description: This function create a new instance of chunk.
 *
 * Param      : None.
 *
 * Return     : The pointer to new instance of chunk.
 * ---------------------------------------------------------------------------
 */
ChunkPtr new_chunk();

#endif //WEBSWITCH_CHUNK_H