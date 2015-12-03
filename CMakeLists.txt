cmake_minimum_required(VERSION 3.2)
project(WebSwitch C)

FIND_PACKAGE (Threads)

#[[set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra")]]
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -fno-strict-aliasing -pthread")
#[[set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -Wno-unused-parameter -pthread")]]

set(SOURCE_FILES_MAIN
        code/main.c
        code/config/heimdall_config.c
        code/web_switch/worker.c
        code/web_switch/watchdog.c
        code/http/connection.c
        code/http/http_response.c
        code/http/http_request.c
        code/utils/log.c
        code/utils/helper.c
        code/utils/throwable.c
        code/web_switch/apache_status.c
        code/web_switch/thread_pool.c
        code/web_switch/worker_pool.c
        code/web_switch/message_controller.c
        code/utils/unix_socket.c
        code/web_switch/request_node.c
        code/web_switch/request_queue.c)

set(SOURCE_FILES_APACHE
        code/utils/helper.c
        code/http/connection.c
        code/utils/log.c
        code/config/heimdall_config.c
        code/utils/throwable.c
        code/web_switch/apache_status.c
        code/http/http_request.c
        code/http/http_response.c)

set(SOURCE_FILES_REQUEST
        code/utils/helper.c
        code/utils/throwable.c
        code/http/http_request.c)

set(SOURCE_FILES_CONFIG
        code/config/heimdall_config.c
        code/config/heimdall_config.conf)

set(SOURCE_FILES_CONNECTION
        code/config/heimdall_config.c
        code/http/connection.c
        code/utils/log.c
        code/utils/throwable.c
        code/http/http_request.c
        code/http/http_response.c
        code/utils/helper.c)

set(SOURCE_FILES_WORKER
        code/web_switch/worker.c
        code/utils/throwable.c
        code/utils/helper.c
        code/http/http_request.c)

set(SOURCE_FILES_WATCHDOG
        code/web_switch/worker.c
        code/web_switch/watchdog.c
        code/utils/throwable.c
        code/utils/helper.c)

set(SOURCE_FILES_WPOOL
        code/web_switch/worker.c
        code/web_switch/worker_pool.c)

set(SOURCE_FILE_REQUEST_QUEUE
        code/config/heimdall_config.c
        code/utils/log.c
        code/utils/throwable.c
        code/utils/helper.c
        code/http/http_request.c
        code/http/http_response.c
        code/web_switch/request_node.c
        code/web_switch/request_queue.c)

set(SOURCE_FILE_SERVER_POOL
        code/config/heimdall_config.c
        code/utils/log.c
        code/utils/throwable.c
        code/utils/helper.c
        code/web_switch/server_pool.c)

set(SOURCE_FILE_CIRCULAR
        code/config/heimdall_config.c
        code/utils/throwable.c
        code/utils/log.c
        code/utils/helper.c
        code/web_switch/circular.c)

set(SOURCE_FILE_ROUND_ROBIN
        code/config/heimdall_config.c
        code/utils/throwable.c
        code/utils/log.c
        code/utils/helper.c
        code/web_switch/circular.c
        code/web_switch/server_pool.c
        code/web_switch/round_robin.c)

add_executable(WebSwitch ${SOURCE_FILES_MAIN})
add_executable(HTTPRequest ${SOURCE_FILES_REQUEST})
add_executable(ConfigParser ${SOURCE_FILES_CONFIG})
add_executable(Connection ${SOURCE_FILES_CONNECTION})
add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
add_executable(Worker ${SOURCE_FILES_WORKER})
add_executable(Watchdog ${SOURCE_FILES_WATCHDOG})
add_executable(Main ${SOURCE_FILES_MAIN})
add_executable(WorkerPool ${SOURCE_FILES_WPOOL})
add_executable(RequestQueue ${SOURCE_FILE_REQUEST_QUEUE})
add_executable(ServerPool ${SOURCE_FILE_SERVER_POOL})
add_executable(Circular ${SOURCE_FILE_CIRCULAR})
add_executable(RoundRobin ${SOURCE_FILE_ROUND_ROBIN})
