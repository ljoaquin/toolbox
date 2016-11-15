cmake_minimum_required(VERSION 2.8)

project(toolbox)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(echo echo.cc socketutil.cc)

add_executable(client client.cc socketutil.cc)

if(WIN32)
	target_link_libraries(echo wsock32 ws2_32)
	target_link_libraries(client wsock32 ws2_32)
endif()
