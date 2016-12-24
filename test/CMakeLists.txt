cmake_minimum_required(VERSION 2.8)

project(test)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(test_thread test_thread.cc)
add_executable(ClienNetwork test_client_network.cc
                            ClientNetwork.cc
                            ${CMAKE_SOURCE_DIR}/socket_util.cc
                            ${CMAKE_SOURCE_DIR}/debug_util.cc)
