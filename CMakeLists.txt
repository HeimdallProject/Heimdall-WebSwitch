cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES
    code/connection.h
    code/helper.h
    code/hello_world.c)

set(SOURCE_FILES_APACHE
    code/connection.h
    code/helper.h
    code/apache_status.c)

add_executable(WebSwitch ${SOURCE_FILES})
add_executable(ApacheStatus ${SOURCE_FILES_APACHE})