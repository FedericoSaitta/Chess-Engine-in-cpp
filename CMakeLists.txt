cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

## Debug builds
#set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions(DEBUG)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra -pedantic")


## Release builds
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")

include_directories(include)
include_directories(tests)

add_executable(ChessEngine src/main.cpp
        src/main.cpp
        include/movegen.h
        src/movegen.cpp
        src/board.cpp
        src/update.cpp
        src/init.cpp
        src/misc.cpp
        include/misc.h
        tests/bitmask_tests.cpp
        tests/tests.h
        src/search.cpp
        include/macros.h
        tests/move_tests.cpp
        tests/perft_tests.cpp
        src/UCI.cpp
        include/evaluation.h
        tests/puzzle_tests.cpp
        src/hashtable.cpp
        include/search.h
        include/uci.h
        include/hashtable.h
        include/init.h
        include/board.h
        include/update.h
        include/inline_functions.h
        src/evaluation.cpp
)
