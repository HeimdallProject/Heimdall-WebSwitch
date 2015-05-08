cmake_minimum_required(VERSION 2.8.4)
project(webswitch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES hello_world.c)
add_executable(webswitch ${SOURCE_FILES})