#include "../include/scheduler.h"


ServerPtr get_ready_server(RRobinPtr rrobin) {
    // preparing the struct tor return
    ServerPtr server = malloc(sizeof(Server));
    if (server == NULL) {
        get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in memory allocation in get_ready_server");
        return NULL;
    }

    // retrieving server
    *server = *(rrobin->get_server(rrobin->circular));
    return server;
}

ThrowablePtr apache_score(ServerNodePtr server) {
    // throwable aux variable
    ThrowablePtr throwable;

    // initializing apache_status struct
    ApacheServerStatusPtr apache_status = new_apache_server_status();
    throwable = apache_status->set_url(apache_status, server->host_address);
    if (throwable->is_an_error(throwable)) {
        throwable = get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in apache_score / set url");
        return throwable;
    }

    // retrieving status from remote Apache machine
    throwable = apache_status->retrieve(apache_status);
    if (throwable->is_an_error(throwable)) {
        throwable = get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in apache_score / retrieve");
        return throwable;
    }
    // ... and parsing result
    throwable = apache_status->parse(apache_status);
    if (throwable->is_an_error(throwable)) {
        throwable = get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in apache_score / retrieve");
        return throwable;
    }

    // checking for correct parsing
    if (apache_status->idle_workers < 0)
        return get_throwable()->create(STATUS_OK, NULL, "apache_score - no updated");

    /* APACHE STATUS SCORE ANALYZER */
    // presetting score utils var
    int score;
    int IDLE_WORKERS  = apache_status->idle_workers;
    int TOTAL_WORKERS = apache_status->busy_workers + IDLE_WORKERS;

    // calculating and setting score - mapping in [1, 5]
    score = (IDLE_WORKERS   - WEIGHT_DEFAULT)   *
            (WEIGHT_MAXIMUM - WEIGHT_DEFAULT)   /
            (TOTAL_WORKERS  - WEIGHT_DEFAULT)   +   WEIGHT_DEFAULT;
    server->weight = score;

    return get_throwable()->create(STATUS_OK, NULL, "apache_score");
}

void *update_server_routine(void *arg) {
    // retrieving argument
    SchedulerPtr scheduler = (SchedulerPtr) arg;

    // retrieving first timestamp
    time_t timestamp = time(NULL);

    // entering for loop TODO: necessary config value!
    int i, proceed;
    ThrowablePtr throwable;
    ServerNodePtr node;
    for(;;)
        if (time(NULL) - timestamp > 1000) {
            // initializing flag
            proceed = 0;
            // scanning across the serverpool
            node = scheduler->server_pool->head;
            for (i = 0; i < scheduler->server_pool->num_servers; i++) {
                // updating weights
                throwable = apache_score(node);
                if (throwable->is_an_error(throwable))
                    proceed -= 1;
                // stepping across pool
                node = node->next;
                if (node == NULL) break;
            }

            // if routine is not failed...
            if (proceed == 0) {
                throwable = scheduler->rrobin->reset(scheduler->rrobin,
                                                     scheduler->server_pool,
                                                     scheduler->server_pool->num_servers);

                if (throwable->is_an_error(throwable)) {
                    get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in RoundRobin resetting");
                    return NULL;
                }
            }

            // updating timestamp
            timestamp = time(NULL);
        }

    return NULL;

}

int main() {
    ServerNodePtr node = malloc(sizeof(ServerNode));
    node->host_address = "www.laziobus.it";
    node->weight = WEIGHT_DEFAULT;

    apache_score(node);
    fprintf(stdout, "SCORE: %d\n", node->weight);

    return 0;
}