#include "../include/circular.h"

// allocating circular buffer structures
ThrowablePtr allocate_buffer(CircularPtr circular, Server **servers, int len) {

    if (*servers == NULL || len == 0) {
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "allocate_buffer");
    }

    ThrowablePtr throwable;

    // entering critical region to modify servers dataset
    throwable = circular->acquire(circular);
    if (throwable->is_an_error(throwable)) {
        get_log()->e(TAG_CIRCULAR, "Error acquiring circular buffer");
        return throwable;
    }

    // allocating the buffer and setting params
    circular->buffer = *servers;
    circular->buffer_len = len;

    circular->head = circular->buffer;
    circular->tail = circular->buffer + len - 1;

    // exiting critical region
    throwable = circular->release(circular);
    if (throwable->is_an_error(throwable)) {
        get_log()->e(TAG_CIRCULAR, "Error releasing circular buffer");
        return throwable;
    }

    return get_throwable()->create(STATUS_OK, NULL, "allocate_buffer");
}

// managing the buffer progress updates
// WARNING: in this implementation there are two main features:
// - tail is useless 'cause the head will continue to check for all the servers (polling behaviour)
//   but it can be used to retrieve externally the ready server (when the function returns a PROGRESS_OK)
// - the circular buffer thread is the only one accessing the circular buffer memory -> let the progress()
//   be an atomic function using the mutex associated to the circular buffer structer (the user must handle the release)
void progress(CircularPtr circular) {

    // recomputing tail, head and buffer position
    circular->tail            = circular->head;
    circular->buffer_position = (circular->buffer_position + 1) % circular->buffer_len;
    circular->head            = circular->buffer + circular->buffer_position;
}

// destroying the circular buffer structures
void destroy_buffer(CircularPtr circular) {

    // destroying mutex
    pthread_mutex_destroy(&circular->mutex);

    // freeing servers struct and ...
    free(circular->buffer);

    // ... finally
    free(circular);
}




// setting up the circular buffer
CircularPtr new_circular(void) {
    CircularPtr circular = malloc(sizeof(Circular));
    if (circular == NULL) {
        get_log()->e(TAG_CIRCULAR, "Memory allocation error in new_circular!");
        exit(EXIT_FAILURE);
    }

    // presetting buffer and buffer_position
    circular->buffer = NULL;
    circular->buffer_position = 0;

    // initializing the mutex
    if (pthread_mutex_init(&circular->mutex, NULL) != 0) {
        get_log()->e(TAG_CIRCULAR, "pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    // setting "methods"
    circular->allocate_buffer = allocate_buffer;
    circular->acquire         = acquire_circular;
    circular->release         = release_circular;
    circular->progress        = progress;
    circular->destroy_buffer  = destroy_buffer;

    return circular;
}


// acquiring the lock associated to the singleton
ThrowablePtr acquire_circular(CircularPtr circular) {
    // getting the lock to enter the critical region
    int mtx = pthread_mutex_lock(&circular->mutex);
    if (mtx != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "pthread_mutex_lock in get_circular");
    else
        return get_throwable()->create(STATUS_OK, NULL, "pthread_mutex_lock in get_circular");
}

// releasing the lock associated to the singleton
ThrowablePtr release_circular(CircularPtr circular) {
    int mtx = pthread_mutex_unlock(&circular->mutex);
    if (mtx != 0)
        return get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "pthread_mutex_unlock in release_circular");
    else
        return get_throwable()->create(STATUS_OK, NULL, "pthread_mutex_unlock in release_circular");
}


// how to use it
/*int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stdout, "Usage %s", argv[0]);
        return -1;
    }

    //get_log()->d(TAG_CIRCULAR, "allocating test array...");
    // initalizing a test array
    Server *servers = malloc(sizeof(Server)*3);
    char *names[3] = {"moretti0.org", "moretti1.org", "moretti2.org"};

    int i;
    for (i = 0; i < 3; i++) {
        servers[i].address = names[i];
        servers[i].status  = SERVER_STATUS_READY;
    }

    //get_log()->d(TAG_CIRCULAR, "creating circular buffer...");
    CircularPtr circular = new_circular();
    circular->allocate_buffer(circular, &servers, 3);

    char *address;

    int k;
    for(k = 0; k < 6; k++) {
        // ACQUIRING -> start critical region
        acquire_circular(circular);
        //get_log()->d(TAG_CIRCULAR, "ACQUIRED\n");
        int buffer_progress = circular->progress(circular);
        if (buffer_progress != BUFFER_PROGRESS_STOP) {
            // simulating the retrieving of the tail in the critical region
            address = circular->tail->address;
            //get_log()->d(TAG_CIRCULAR, "BUFFERING: %s\n", address);
        } else {
            // simulating the service routine for server broken
            Server *serv = circular->head;
            if (serv->status == SERVER_STATUS_BROKEN) {
                //get_log()->d(TAG_CIRCULAR, "Starting broken routine");
            }
        }

        // RELEASING -> end critical region
        release_circular(circular);
    }

    circular->destroy_buffer(circular);
    fprintf(stdout, "BUFFER DESTROYED\n");
    fprintf(stdout, "STOPPED: max retries limit reached!\n");
    return 0;
}*/