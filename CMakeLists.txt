cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra -pedantic")

add_executable(ChessEngine src/main.cpp
        src/main.cpp
        src/movegen.cpp
        src/movegen.h
        src/board.cpp
        src/board.h
        src/globals.h
        src/attack.cpp
        src/attack.h
        src/update.cpp)
