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

include_directories(include)
include_directories(benchmark_tests)
include_directories(debug_tests)

add_executable(ChessEngine src/main.cpp
        src/main.cpp
        include/movegen.h
        src/movegen.cpp
        src/board.cpp
        src/update.cpp
        src/init.cpp
        src/misc.cpp
        include/misc.h
        debug_tests/bitmask_tests.cpp
        benchmark_tests/benchmark_tests.h
        src/search.cpp
        include/macros.h
        debug_tests/move_tests.cpp
        benchmark_tests/perft_tests.cpp
        src/UCI.cpp
        include/evaluation.h
        benchmark_tests/puzzle_tests.cpp
        src/hashtable.cpp
        include/search.h
        include/uci.h
        include/hashtable.h
        include/init.h
        include/board.h
        include/update.h
        include/inline_functions.h
        src/evaluation.cpp
        debug_tests/evaluation_tests.cpp
        debug_tests/debug_tests.h
        benchmark_tests/branching_factor.cpp
        src/movesort.cpp
        include/movesort.h
)
