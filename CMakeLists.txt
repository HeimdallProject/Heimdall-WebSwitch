cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra")

set(SOURCE_FILES_MAIN
    code/hello_world.c)

set(SOURCE_FILES_APACHE
    code/connection.h
    code/connection.c
    code/helper.h
    code/helper.c
    code/apache_status.c)

set(SOURCE_FILES_REQUEST
    code/http_request.h
    code/helper.h
    code/helper.c
    code/http_request.c)

add_executable(WebSwitch ${SOURCE_FILES_MAIN})
add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
add_executable(HTTPRequest ${SOURCE_FILES_REQUEST})