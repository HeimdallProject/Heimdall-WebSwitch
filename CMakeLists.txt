cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra")

set(SOURCE_FILES
    code/connection.h
    code/helper.h
    code/hello_world.c)

set(SOURCE_FILES_APACHE
    code/connection.h
    code/helper.h
    code/apache_status.c)

set(SOURCE_FILES_HTTP_REQUEST
    code/request.h
    code/HTTP_request.c)

add_executable(WebSwitch ${SOURCE_FILES})
add_executable(ApacheStatus ${SOURCE_FILES_APACHE})
add_executable(HTTP_Request ${SOURCE_FILES_HTTP_REQUEST})