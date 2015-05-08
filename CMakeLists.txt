cmake_minimum_required(VERSION 3.2)
project(WebSwitch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    code/hello_world.c)

add_executable(WebSwitch ${SOURCE_FILES})