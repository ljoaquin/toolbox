cmake_minimum_required(VERSION 2.8)

project(test)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(test_format  test_format.cc
                            ${CMAKE_SOURCE_DIR}/string_format.cc)
add_executable(test_variadic_template test_variadic_template.cc)
add_executable(test_thread test_thread.cc)
add_executable(test_client_network test_client_network.cc
                            client_network.cc
                            ${CMAKE_SOURCE_DIR}/socket_util.cc
                            ${CMAKE_SOURCE_DIR}/debug_util.cc)
