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
        throwable = get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "malloc in weighted_servers");
        return throwable;
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
    if (throwable->is_an_error(throwable))
        throwable = get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "allocate_buffer in weight_servers");
    else
        throwable = get_throwable()->create(STATUS_OK, NULL, "weight_servers");

    return throwable;
}


ThrowablePtr reset_servers(RRobinPtr rrobin, ServerPoolPtr pool, int server_num) {

    // freeing the old buffer if present
    if (rrobin->circular->buffer != NULL)
        free(rrobin->circular->buffer);

    ThrowablePtr throwable;

    // creating auxiliary buffer
    Server *servers = malloc(sizeof(Server) * pool->num_servers);
    if (servers == NULL) {
        throwable = get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "allocate_buffer in reset_servers");
        return throwable;
    }
    // initializing auxiliary buffer
    ServerNodePtr node = pool->head;
    int s;
    for (s = 0; s < pool->num_servers; s++) {
        (servers + s)->address = node->host_address;
        (servers + s)->ip      = node->host_ip     ;
        (servers + s)->port    = node->port_number ;
        (servers + s)->weight  = node->weight      ;
        node = node->next;
    }

    // routine to modify servers dataset
    throwable = rrobin->weight(rrobin->circular, servers, server_num);
    // freeing auxiliary buffer
    free(servers);
    // checking for weighting procedure
    if (throwable->is_an_error(throwable)) {
        get_log()->e(TAG_ROUND_ROBIN, "Resetting RRobin");
        return throwable;
    } else
        return get_throwable()->create(STATUS_OK, NULL, "Resetting RRobin");
}


Server *get_server(CircularPtr circular) {

    ThrowablePtr throwable;
    Server *server_ready;

    // entering critical region
    throwable = circular->acquire(circular);
    if (throwable->is_an_error(throwable)) {
        get_log()->e(TAG_ROUND_ROBIN, "Error acquiring circular buffer");
        return NULL;
    }

    // performing progressing step
    circular->progress(circular);

    // retrieving server
    server_ready = circular->tail;


    // exiting critical region
    throwable = circular->release(circular);
    if (throwable->is_an_error(throwable)) {
        get_log()->e(TAG_ROUND_ROBIN, "Error releasing circular buffer");
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
        get_log()->d(TAG_ROUND_ROBIN, (rrobin->circular->buffer + i)->address);

    fprintf(stdout, "-------------------------------------------------\n\n");
    fprintf(stdout, "EXECUTE!\n");

    for(i = 0; i < rrobin->circular->buffer_len; i++)
        get_log()->d(TAG_ROUND_ROBIN, rrobin->get_server(rrobin->circular)->address);

    return 0;
} */