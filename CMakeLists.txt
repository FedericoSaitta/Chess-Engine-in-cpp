cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

## Debug builds
set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra -pedantic")


## Release builds
#et(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")

include_directories(include)
include_directories(tests)

add_executable(ChessEngine src/main.cpp
        src/main.cpp
        src/movegen.cpp
        src/board.cpp
        include/globals.h
        src/update.cpp
        src/init.cpp
        src/misc.cpp
        include/misc.h
        tests/bitmask_tests.cpp
        tests/tests.h
        include/constants.h
        include/inline_functions.h
        src/globals.cpp
        tests/move_tests.cpp
)
