#include "../include/scheduler.h"


ServerPtr get_ready_server(RRobinPtr rrobin) {
    // preparing the struct tor return
    ServerPtr server = malloc(sizeof(Server));
    if (server == NULL) {
        get_log()->e(TAG_SCHEDULER, "Memory allocation error!", "get_ready_server");
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
        return throwable->thrown(throwable, "apache_score");
    }

    // retrieving status from remote Apache machine
    throwable = apache_status->retrieve(apache_status);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "apache_score");
    }
    // ... and parsing result
    throwable = apache_status->parse(apache_status);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "apache_score");
    }

    // checking for correct parsing
    if (apache_status->idle_workers < 0) {
        return get_throwable()->create(STATUS_OK, "No update", "apache_score");
    }

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
    time_t up_since  = time(NULL);
    // retrieving awaiting time TODO: necessary config value!
    time_t up_time   = 1000;

    // entering for loop
    int i, proceed;
    ThrowablePtr throwable;
    ServerNodePtr node;
    for(;;)
        if (time(NULL) - up_since > up_time) {
            // initializing flag
            proceed = 0;
            // scanning across the serverpool
            node = scheduler->server_pool->head;
            for (i = 0; i < scheduler->server_pool->num_servers; i++) {
                // updating weights
                throwable = apache_score(node);
                if (throwable->is_an_error(throwable)) {
                    get_log->t(throwable);
                    proceed -= 1;
                }
                // stepping across pool
                node = node->next;
                if (node == NULL) 
                    break;
            } 

            // if routine is not failed...
            if (proceed == 0) {
                throwable = scheduler->rrobin->reset(scheduler->rrobin, scheduler->server_pool, scheduler->server_pool->num_servers);
                if (throwable->is_an_error(throwable)) {
                    get_log()->t(throwable);
                    return NULL;
                }
            }

            // updating timestamp
            up_since = time(NULL);
        }

    return NULL;

}


SchedulerPtr init_scheduler(int awareness_level) {
    // TODO: retrieving from configuration the server list, now assuming we have them as a list of string
    char *servers_addresses[3] = {"alessiomoretti.it", "alessiomoretti.it", "alessiomoretti.it"};
    char *servers_ip[3] = {"12.34.56.78", "12.34.56.78", "12.34.56.78"};
    int n = 3;

    // allocating memory - scheduler
    SchedulerPtr scheduler = malloc(sizeof(Scheduler));
    if (scheduler == NULL) {
        get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "init_scheduler");
        return NULL;
    }
    // allocating memory - rrobin
    scheduler->rrobin = new_rrobin();

    // allocating memory - server pool
    scheduler->server_pool = init_server_pool();

    // in server pool adding server nodes
    int i;
    ServerNodePtr node;
    for (i = 0; i < n; i++) {
        node = malloc(sizeof(ServerNode));
        if (node == NULL) {
            get_throwable()->create(STATUS_ERROR, "Memory allocation error!", "init_scheduler");
            return NULL;
        }

        node->host_address = servers_addresses[i];
        node->host_ip      = servers_ip[i];
        node->weight       = WEIGHT_DEFAULT;

        scheduler->server_pool->add_server(scheduler->server_pool, node);
    }

    // setting round robin
    ThrowablePtr throwable = scheduler->rrobin->reset(scheduler->rrobin, scheduler->server_pool, scheduler->server_pool->num_servers);
    if (throwable->is_an_error(throwable)) {
        get_log()->t(throwable);
        return NULL;
    }

    // setting "methods"
    scheduler->get_server = get_ready_server;

    // if it is a state-aware discipline, detaching a new thread
    if (awareness_level == AWARENESS_LEVEL_HIGH) {
        // TODO: detach a new thread to update round robin
    }

    return scheduler;
}
