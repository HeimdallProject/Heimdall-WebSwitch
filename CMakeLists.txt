cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra")

set(SOURCE_FILES_MAIN
    code/main.c)

set(SOURCE_FILES_APACHE
    code/helper.c
    code/connection.c
    code/log.c
    code/throwable.c
    code/apache_status.c)

set(SOURCE_FILES_REQUEST
    code/helper.c
    code/http_request.c)

set(SOURCE_FILES_LOG
    code/log.c
    code/helper.c)

set(SOURCE_FILES_THROWABLE
    code/throwable.c
    code/throwable.h
    code/log.c
    code/helper.c)

set(SOURCE_FILES_CONFIG
    code/config_parser.c
    heimdall_config.conf)

add_executable(WebSwitch ${SOURCE_FILES_MAIN})
add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
add_executable(HTTPRequest ${SOURCE_FILES_REQUEST})
add_executable(Log ${SOURCE_FILES_LOG})
add_executable(Throwable ${SOURCE_FILES_THROWABLE})
add_executable(ConfigParser ${SOURCE_FILES_CONFIG})