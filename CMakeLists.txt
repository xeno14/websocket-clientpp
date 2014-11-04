project(websocket-clientpp)

cmake_minimum_required(VERSION 2.8)

#TODO: Compiler check
set(CMAKE_CXX_COMPILER g++-4.8)
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_BUILD_TYPE Release)
add_definitions("-Wall -std=c++11")

add_subdirectory(test)

add_library(websocket-clientpp websocket-clientpp.cpp)

add_executable(example sync-client-example.cpp)
target_link_libraries(example websocket-clientpp)
