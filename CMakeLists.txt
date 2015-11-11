cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra")

set(SOURCE_FILES_MAIN
    code/main.c)

set(SOURCE_FILES_APACHE
        code/utils/helper.c
        code/HTTP/connection.c
        code/utils/log.c
        code/utils/throwable.c
        code/web_switch/apache_status.c)

set(SOURCE_FILES_REQUEST
        code/utils/helper.c
        code/HTTP/http_request.c)

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

#add_executable(WebSwitch ${SOURCE_FILES_MAIN})
#add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
#add_executable(HTTPRequest ${SOURCE_FILES_REQUEST})
#add_executable(Log ${SOURCE_FILES_LOG})
#add_executable(Throwable ${SOURCE_FILES_THROWABLE})
#add_executable(ConfigParser ${SOURCE_FILES_CONFIG})
#add_executable(Connection ${SOURCE_FILES_CONNECTION})
add_executable(Connection ${SOURCE_FILES_WORKER})
