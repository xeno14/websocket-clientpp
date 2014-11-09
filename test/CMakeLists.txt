enable_testing()
macro(add_gtest test_name src libs)
  add_executable(${test_name} ${src})
  target_link_libraries(${test_name} gtest gtest_main ${libs})
  add_test(${test_name} ${CMAKE_CURRENT_BINARY_DIR}/${test_name})
endmacro(add_gtest)

add_gtest(websocket-clientpp_test ./websocket-clientpp_test.cpp websocket-clientpp)
add_gtest(protocol_test ./protocol_test.cpp websocket-clientpp)
