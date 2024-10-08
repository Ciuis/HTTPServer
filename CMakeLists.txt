cmake_minimum_required(VERSION 3.21)
project(HTTPserver C)

set(CMAKE_C_STANDARD 17)

add_executable(HTTPserver src/server.c)

target_link_libraries(HTTPserver ws2_32)