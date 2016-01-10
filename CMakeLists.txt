project(websocket-clientpp)

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_BUILD_TYPE Release)
add_definitions("-Wall -std=c++11 -static")

enable_testing()
macro(add_gtest test_name src libs)
  add_executable(${test_name} ${src})
  target_link_libraries(${test_name} gtest gtest_main ${libs})
  add_test(${test_name} ${CMAKE_CURRENT_BINARY_DIR}/${test_name})
endmacro(add_gtest)

add_subdirectory(test)

add_library(websocket-clientpp websocket-clientpp.cpp)

add_executable(example sync-client-example.cpp)
target_link_libraries(example websocket-clientpp)
