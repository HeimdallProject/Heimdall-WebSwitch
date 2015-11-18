cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

FIND_PACKAGE (Threads)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra")
#[[set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -pthread")]]

set(SOURCE_FILES_MAIN
        code/main.c
        code/config/config_parser.c
        code/web_switch/thread_pool.c
        code/web_switch/worker.c
        code/web_switch/worker_pool.c
        code/utils/log.c
        code/utils/helper.c
        code/utils/throwable.c)

set(SOURCE_FILES_APACHE
        code/utils/helper.c
        code/http/connection.c
        code/utils/log.c
        code/utils/throwable.c
        code/web_switch/apache_status.c)

set(SOURCE_FILES_REQUEST
        code/utils/helper.c
        code/http/http_request.c)

set(SOURCE_FILES_LOG
        code/utils/log.c
        code/utils/helper.c)

set(SOURCE_FILES_THROWABLE
        code/utils/throwable.c
        code/utils/log.c
        code/utils/helper.c)

set(SOURCE_FILES_CONFIG
        code/config/config_parser.c
        heimdall_config.conf)

set(SOURCE_FILES_CONNECTION
        code/http/connection.c
        code/utils/log.c
        code/utils/throwable.c
        code/http/http_request.c
        code/utils/helper.c)

set(SOURCE_FILES_WORKER
        code/web_switch/worker.c
        code/utils/throwable.c
        code/utils/helper.c
        code/http/http_request.c)

set(SOURCE_FILES_WPOOL
        code/web_switch/worker.c
        code/web_switch/worker_pool.c)

set(SOURCE_FILE_REQUEST_QUEUE
        code/utils/log.c
        code/utils/throwable.c
        code/utils/helper.c
        code/http/http_request.c
        code/http/http_response.c
        code/utils/request_node.c
        code/utils/request_queue.c)

add_executable(WebSwitch ${SOURCE_FILES_MAIN})
add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
add_executable(HTTPRequest ${SOURCE_FILES_REQUEST})
add_executable(Log ${SOURCE_FILES_LOG})
add_executable(Throwable ${SOURCE_FILES_THROWABLE})
add_executable(ConfigParser ${SOURCE_FILES_CONFIG})
add_executable(Connection ${SOURCE_FILES_CONNECTION})
add_executable(Worker ${SOURCE_FILES_WORKER})
add_executable(Main ${SOURCE_FILES_MAIN})
add_executable(WorkerPool ${SOURCE_FILES_WPOOL})
add_executable(RequestQueue ${SOURCE_FILE_REQUEST_QUEUE})
