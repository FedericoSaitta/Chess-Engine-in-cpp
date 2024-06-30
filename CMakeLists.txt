cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

## Debug builds
set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra -pedantic")


## Release builds
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")



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
