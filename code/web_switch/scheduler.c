#include "../include/scheduler.h"

// singleton to the scheduler
static SchedulerPtr scheduler_singleton = NULL;


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
    /*get_log()->d(TAG_SCHEDULER, "scoring routine...");*/
    // throwable aux variable
    ThrowablePtr throwable;

    // initializing apache_status struct
    ApacheServerStatusPtr apache_status = new_apache_server_status();
    throwable = apache_status->set_url(apache_status, server->host_ip);
    if (throwable->is_an_error(throwable)) {
        return throwable->thrown(throwable, "apache_score");
    }

    // retrieving status from remote Apache machine
    throwable = apache_status->retrieve(apache_status);
    if (throwable->is_an_error(throwable)) {
        server->weight = WEIGHT_DEFAULT;
        server->status = SERVER_STATUS_BROKEN;
        get_log()->d(TAG_SCHEDULER, "SERVER %s is DOWN", server->host_ip);
        return throwable->thrown(throwable, "apache_score");
    } else {
        server->status = SERVER_STATUS_READY;
        get_log()->d(TAG_SCHEDULER, "SERVER %s is UP", server->host_ip);
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

    // calculating and setting score - mapping in [w, W]
    score = (IDLE_WORKERS   - WEIGHT_DEFAULT)   *
            (WEIGHT_MAXIMUM - WEIGHT_DEFAULT)   /
            (TOTAL_WORKERS  - WEIGHT_DEFAULT)   +   WEIGHT_DEFAULT;
    server->weight = score;
    /*get_log()->d(TAG_SCHEDULER, "[SCORE: %d (%s)]", score, server->host_ip);*/

    return get_throwable()->create(STATUS_OK, NULL, "apache_score");
}

void *update_server_routine(void *arg) {

    /*get_log()->d(TAG_SCHEDULER, "Scheduler update routine init...");*/

    // initiliazing throwable
    ThrowablePtr throwable;

    // retrieving argument
    SchedulerPtr scheduler = (SchedulerPtr) arg;

    ConfigPtr config = get_config();

    // retrieving update time from config
    time_t up_time = 10;
    throwable = str_to_long(config->update_time, &up_time);
    if (throwable->is_an_error(throwable)) {
        get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in configuration parsing");
        return NULL;
    }

    // retrieving first timestamp
    time_t up_since  = time(NULL);
    // entering for loop
    int i, proceed;
    ServerNodePtr node;
    for(;;)
        if (time(NULL) - up_since > up_time) {
            /*get_log()->d(TAG_SCHEDULER, "Scheduler update routine [UPDATING]");*/
            // initializing flag
            proceed = 0;
            // scanning across the serverpool
            node = scheduler->server_pool->head;
            for (i = 0; i < scheduler->server_pool->num_servers; i++) {
                // updating weights
                throwable = apache_score(node);
                if (throwable->is_an_error(throwable)) {
                    get_log()->t(throwable);
                    get_log()->d(TAG_SCHEDULER, "NODE STATUS %d", node->status);
                    if (node->status != SERVER_STATUS_BROKEN)
                        proceed -= 1;
                }

                // stepping across pool
                node = node->next;
                if (node == NULL) 
                    break;
            } 

            // if routine is not failed...
            if (proceed == 0) {
                get_log()->d(TAG_SCHEDULER, "SCHEDULING...");
                throwable = scheduler->rrobin->reset(scheduler->rrobin, scheduler->server_pool, scheduler->server_pool->num_servers);
                if (throwable->is_an_error(throwable)) {
                    get_log()->t(throwable);
                    return NULL;
                }
            }
            get_log()->d(TAG_SCHEDULER, "Scheduler update routine [UPDATE COMPLETE]");
            // updating timestamp
            up_since = time(NULL);
        }

    return NULL;

}


SchedulerPtr init_scheduler(int awareness_level) {

    ServerConfigPtr server_config = get_server_config();
    int n = server_config->total_server;

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

        get_log()->d(TAG_SCHEDULER, "Server n° %d Name %s, IP %s", i, server_config->servers_names[i], server_config->servers_ip[i]);

        node->host_address = server_config->servers_names[i];
        node->host_ip      = server_config->servers_ip[i];
        node->weight       = WEIGHT_DEFAULT;
        node->status       = SERVER_STATUS_READY;

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
    int state_aware_init;
    pthread_t updater;
    if (awareness_level == AWARENESS_LEVEL_HIGH) {
        // updater thread creation
        state_aware_init = pthread_create(&updater, NULL, update_server_routine, (void *) scheduler);
        if (state_aware_init != 0) {
            get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in pthread creation - init_scheduler");
            return NULL;
        }

        // updater thread detachment
        state_aware_init = pthread_detach(updater);
        if (state_aware_init != 0) {
            get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in pthread detachment - init_scheduler");
            return NULL;
        }
    }

    return scheduler;
}


SchedulerPtr get_scheduler() {

    // checking wheter scheduler singleton is initialized or not
    if (scheduler_singleton != NULL) {
        return scheduler_singleton;
    } else {
        
        ConfigPtr config = get_config();

        // getting awareness level from configuration file
        int awareness = 0;     
        ThrowablePtr throwable = str_to_int(config->algorithm_selection, &awareness);
        if (throwable->is_an_error(throwable)) {
            get_log()->t(throwable);
            exit(EXIT_FAILURE);
        }

        // initializing scheduler
        switch (awareness) {
            case AWARENESS_LEVEL_LOW:
                scheduler_singleton = init_scheduler(AWARENESS_LEVEL_LOW);
                if (scheduler_singleton == NULL) {
                    get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in get_scheduler - init_scheduler");
                    exit(EXIT_FAILURE);
                }
                break;

            case AWARENESS_LEVEL_HIGH:
                scheduler_singleton = init_scheduler(AWARENESS_LEVEL_HIGH);
                if (scheduler_singleton == NULL) {
                    get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in get_scheduler - init_scheduler");
                    exit(EXIT_FAILURE);
                }
                break;

            default:
                if (scheduler_singleton == NULL) {
                    get_throwable()->create(STATUS_ERROR, get_error_by_errno(errno), "Error in get_scheduler - conf parsing");
                    exit(EXIT_FAILURE);
                }
        }

        // returning pointer to scheduler
        return scheduler_singleton;
    }
}

