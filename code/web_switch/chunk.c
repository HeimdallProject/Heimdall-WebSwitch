#include "../include/chunk.h"

ChunkPtr new_chunk() {

    // Allocs memory
    ChunkPtr chunk = malloc(sizeof(Chunk));
    if (chunk == NULL) {
        get_log()->e(TAG_CHUNK, "Memory allocation error in new_chunk!");
        exit(EXIT_FAILURE);
    }

    // Initializes mutex
    if (pthread_mutex_init(&(chunk->mutex), NULL) != 0) {
        get_log()->e(TAG_CHUNK, get_error_by_errno(errno));
        exit(EXIT_FAILURE);
    }

    // Initializes chunk
    chunk->wrote = TRUE;
    chunk->dimen = 0;
    chunk->data = malloc(MAX_CHUNK_SIZE);
    if (chunk == NULL) {
        get_log()->e(TAG_CHUNK, "Memory allocation error in new_chunk!");
        exit(EXIT_FAILURE);
    }

    return chunk;
}