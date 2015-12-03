#include "../include/server_pool.h"


void add_server(ServerPoolPtr pool, ServerNodePtr new_node) {

    // if there is no server in the pool
    if (pool->head == NULL) {
        pool->head = new_node;
        pool->tail = new_node;
        new_node->next = NULL;
    } else {
        pool->tail->next = new_node;
        pool->tail = new_node;
        new_node->next = NULL;
    }

    // incrementing server number
    pool->num_servers++;
}

void delete_server(ServerPoolPtr pool, ServerNodePtr del_node) {

    // retrieving the previous node before the deleting one
    ServerNodePtr server = pool->head;
    while (server->next != del_node) server = server->next;

    // if the deleting node is the tail ...
    if (pool->tail == del_node)
        pool->tail = server;

    // resetting previous node pointer
    server->next = del_node->next;

    // freeing memory
    free(del_node);
}



ServerPoolPtr init_server_pool(void) {

    // allocating memory
    ServerPoolPtr pool = malloc(sizeof(ServerPool));
    if (pool == NULL) {
        get_log()->e(TAG_SERVER_POOL, "Error in memory allocation");
        exit(EXIT_FAILURE);
    }

    // presetting structure attributes
    pool->head = NULL;
    pool->tail = NULL;
    pool->num_servers = 0;

    // setting structure "methods"
    pool->add_server = add_server;
    pool->del_server = delete_server;

    return pool;
}

/* use case
int main() {

    ServerPoolPtr pool = init_server_pool();

    ServerNodePtr server1 = malloc(sizeof(ServerNode));
    server1->host_address = "outsidertech0.net";

    ServerNodePtr server2 = malloc(sizeof(ServerNode));
    server2->host_address = "outsidertech1.net";

    ServerNodePtr server3 = malloc(sizeof(ServerNode));
    server3->host_address = "outsidertech2.net";

    pool->add_server(pool, server1);
    pool->add_server(pool, server2);
    pool->add_server(pool, server3);

    fprintf(stdout, "%s ---> %s ---> %s\n", pool->head->host_address, pool->head->next->host_address, pool->head->next->next->host_address);
    fprintf(stdout, "%s ---> %s ---> %s\n", pool->head->host_address, pool->head->next->host_address, pool->tail->host_address);

    fprintf(stdout, "\n-----------------\n\n");

    pool->del_server(pool, server2);
    fprintf(stdout, "%s ---> %s\n", pool->head->host_address, pool->head->next->host_address);
    fprintf(stdout, "%s ---> %s\n", pool->head->host_address, pool->tail->host_address);

    return 0;

} */