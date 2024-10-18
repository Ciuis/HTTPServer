cmake_minimum_required(VERSION 3.21)
project(HTTPserver C)

set(CMAKE_C_STANDARD 17)

# Указываем директорию для объектных файлов
set(OBJDIR ${CMAKE_BINARY_DIR}/obj)

# Указываем директорию для бинарных файлов
set(BINDIR ${CMAKE_BINARY_DIR}/bin)

add_executable(HTTPserver src/server.c include/request.h include/response.h src/request.c src/response.c)
include_directories(include)



target_link_libraries(HTTPserver ws2_32)