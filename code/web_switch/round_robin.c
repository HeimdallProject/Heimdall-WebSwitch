#include "../include/round_robin.h"

// making a weighted scheduling discipline
ThrowablePtr weight_servers(CircularPtr circular, Server *servers, int server_num) {

    // finding the sum for all the servers and sorting them
    int i, j;
    int weight_sum = 0;
    Server temp_serv;
    for (i = 0; i < server_num - 1; i++) {
        weight_sum += (servers + i)->weight;
        for (j = i + 1; j < server_num; j++) {
            if ((servers + i)->weight > (servers + j)->weight) {
                temp_serv = *(servers + i);
                *(servers + i) = *(servers + j);
                *(servers + j) = temp_serv;
            }
        }
    }
    weight_sum += (servers + server_num - 1)->weight;

    // allocating the server pattern sequence
    ThrowablePtr throwable;
    Server *w_servers = malloc(sizeof(Server) * weight_sum);
    if (w_servers == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "weight_servers");
    }

    // weighted pattern creation
    int w = 0;
    int s = 0;
    for (;;) {

        // creating pattern taking each round weight times the server
        if ((servers + s)->weight > 0) {
            *(w_servers + w) = *(servers + s);
            // updating weight info
            (servers + s)->weight -= 1;
            if (++w == weight_sum)
                break;
        }

        // handling for loop
        if (++s == server_num)
            s = 0;
    }

    // creating the circular buffer
    throwable = circular->allocate_buffer(circular, &w_servers, weight_sum);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "weight_servers");
    }

    return get_throwable()->create(STATUS_OK, NULL, "weight_servers");
}


ThrowablePtr reset_servers(RRobinPtr rrobin, ServerPoolPtr pool, int server_num) {

    // freeing the old buffer if present
    if (rrobin->circular->buffer != NULL)
        free(rrobin->circular->buffer);

    ThrowablePtr throwable;

    // creating auxiliary buffer
    Server *servers = malloc(sizeof(Server) * pool->num_servers);
    if (servers == NULL) {
        return get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "reset_servers");
    }
    // initializing auxiliary buffer
    ServerNodePtr node = pool->head;
    int s;
    for (s = 0; s < pool->num_servers; s++) {
        (servers + s)->address = node->host_address;
        (servers + s)->ip      = node->host_ip     ;
        (servers + s)->weight  = node->weight      ;
        (servers + s)->status  = node->status      ;
        node = node->next;
    }

    // routine to modify servers dataset
    throwable = rrobin->weight(rrobin->circular, servers, server_num);
    // freeing auxiliary buffer
    free(servers);
    // checking for weighting procedure
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "reset_servers");
    } 

    return get_throwable()->create(STATUS_OK, NULL, "reset_servers");
}


ServerPtr get_server(CircularPtr circular) {

    ThrowablePtr throwable;

    // allocating server ready struct
    ServerPtr server_ready = malloc(sizeof(Server));
    if (server_ready == NULL) {
        return NULL;
    }

    // entering critical region
    throwable = circular->acquire(circular);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        return NULL;
    }

    // performing progressing step
    circular->progress(circular);
    // retrieving server
    *server_ready = *(circular->tail);

    // iterating while the servr status is not SERVER_STATUS_BROKEN
    // (remote machine is not available)
    int not_available = 0;
    for (;;) {
        // checking if server statys is SERVER_STATUS_READY
        if (server_ready->status == SERVER_STATUS_READY)
            break;

        // stepping the circular buffer
        circular->progress(circular),
        *server_ready = *(circular->tail);
        not_available++;

        // checking if the buffer has not been already scanned
        // if all machines are not available, then return BROKEN as status
        // and closing connection in the worker routine
        if (not_available == circular->buffer_len) {
            get_log()->i(TAG_CIRCULAR, "ALL SERVERS ARE DOWN! - closing connection... now!");
            // returning server with status broken
            server_ready->status = SERVER_STATUS_BROKEN;
            break;
        }
    }


    // exiting critical region
    throwable = circular->release(circular);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        return NULL;
    }

    return server_ready;
}


RRobinPtr new_rrobin() {

    // allocating struct
    RRobinPtr rrobin = malloc(sizeof(RRobin));
    if (rrobin == NULL) {
        get_log()->e(TAG_ROUND_ROBIN, "Memory allocation in new_rrobin!");
        exit(EXIT_FAILURE);
    }

    // allocating circular buffer
    rrobin->circular = new_circular();

    // setting "methods"
    rrobin->weight     = weight_servers;
    rrobin->reset      = reset_servers;
    rrobin->get_server = get_server;

    return rrobin;
}

/* usage
int main() {

    // test servers array
    ServerPoolPtr pool = init_server_pool();

    ServerNodePtr server1 = malloc(sizeof(ServerNode));
    server1->host_address = "outsidertech0.net";
    server1->weight = 1;

    ServerNodePtr server2 = malloc(sizeof(ServerNode));
    server2->host_address = "outsidertech1.net";
    server2->weight = 2;

    ServerNodePtr server3 = malloc(sizeof(ServerNode));
    server3->host_address = "outsidertech2.net";
    server3->weight = 3;

    pool->add_server(pool, server1);
    pool->add_server(pool, server2);
    pool->add_server(pool, server3);

    RRobinPtr rrobin = new_rrobin();
    rrobin->reset(rrobin, pool, 3);

    fprintf(stdout, "------------------ PATTERN RR -------------------\n");
    int i;
    for(i = 0; i < rrobin->circular->buffer_len; i++)
        //get_log()->d(TAG_ROUND_ROBIN, (rrobin->circular->buffer + i)->address);

    fprintf(stdout, "-------------------------------------------------\n\n");
    fprintf(stdout, "EXECUTE!\n");

    for(i = 0; i < rrobin->circular->buffer_len; i++)
        //get_log()->d(TAG_ROUND_ROBIN, rrobin->get_server(rrobin->circular)->address);

    return 0;
} */