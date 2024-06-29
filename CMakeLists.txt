cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra -pedantic")

add_executable(ChessEngine src/main.cpp
        src/main.cpp
        src/movegen.cpp
        src/board.cpp
        src/globals.h
        src/attack.cpp
        src/update.cpp
        src/init.cpp
        src/misc.cpp
        src/misc.h
        tests/bitmask_tests.cpp
        tests/bitmask_tests.h)
