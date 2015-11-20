cmake_minimum_required(VERSION 3.2)
project(WebSwitch C)

FIND_PACKAGE (Threads)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -Wno-unused-parameter -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -Wno-unused-parameter")
#[[set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -pthread")]]

set(SOURCE_FILES_MAIN
        code/main.c
        code/web_switch/config_parser.c
        code/web_switch/thread_pool.c
        code/web_switch/worker_obj.c
        code/web_switch/worker_pool.c
        code/utils/log.c
        code/utils/helper.c
        code/utils/throwable.c)

set(SOURCE_FILES_APACHE
        code/utils/helper.c
        code/HTTP/connection.c
        code/utils/log.c
        code/utils/throwable.c
        code/web_switch/apache_status.c)

set(SOURCE_FILES_REQUEST
        code/utils/log.c
        code/utils/throwable.c
        code/utils/helper.c
        code/HTTP/http_request.c)

set(SOURCE_FILES_RESPONSE
        code/utils/log.c
        code/utils/throwable.c
        code/utils/helper.c
        code/HTTP/http_request.c
        code/HTTP/http_response.c)

set(SOURCE_FILES_LOG
        code/utils/log.c
        code/utils/helper.c)

set(SOURCE_FILES_THROWABLE
        code/utils/throwable.c
        code/utils/log.c
        code/utils/helper.c)

set(SOURCE_FILES_CONFIG
        code/web_switch/config_parser.c
        heimdall_config.conf)

set(SOURCE_FILES_CONNECTION
        code/HTTP/connection.c
        code/utils/log.c
        code/utils/throwable.c
        code/HTTP/http_request.c
        code/utils/helper.c)

set(SOURCE_FILES_WORKER
        code/web_switch/worker.c
        code/utils/throwable.c
        code/utils/helper.c
        code/HTTP/http_request.c)

set(SOURCE_FILES_WPOOL
        code/web_switch/worker_obj.c
        code/web_switch/worker_pool.c)

set(SOURCE_FILE_REQUEST_QUEUE
        code/utils/log.c
        code/utils/throwable.c
        code/utils/helper.c
        code/HTTP/http_request.c
        code/HTTP/http_response.c
        code/utils/request_node.c
        code/utils/request_queue.c)

set(SOURCE_FILE_ROUNDROBIN
        code/utils/throwable.c
        code/web_switch/circular.c
        code/web_switch/round_robin.c)

#add_executable(WebSwitch ${SOURCE_FILES_MAIN})
#add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
#add_executable(HTTPRequest ${SOURCE_FILES_REQUEST})
#add_executable(HTTPResponse ${SOURCE_FILES_RESPONSE})
#add_executable(Log ${SOURCE_FILES_LOG})
#add_executable(Throwable ${SOURCE_FILES_THROWABLE})
#add_executable(ConfigParser ${SOURCE_FILES_CONFIG})
#add_executable(Connection ${SOURCE_FILES_CONNECTION})
#add_executable(Worker ${SOURCE_FILES_WORKER})
#add_executable(Main ${SOURCE_FILES_MAIN})
#add_executable(WorkerPool ${SOURCE_FILES_WPOOL})
#add_executable(RequestQueue ${SOURCE_FILE_REQUEST_QUEUE})
add_executable(RoundRobin ${SOURCE_FILE_ROUNDROBIN})
