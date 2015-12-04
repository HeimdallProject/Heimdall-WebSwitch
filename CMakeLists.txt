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

add_executable(WebSwitch ${SOURCE_FILES_MAIN})
