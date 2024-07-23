cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

# Debug builds (if needed)
#set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions(DEBUG)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall -Wextra -pedantic")

# Release builds
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")

# Include directories
include_directories(include)
include_directories(benchmark_tests)
include_directories(debug_tests)
include_directories(eval)
include_directories(search)
include_directories(movegen)


# Add the executable
add_executable(ChessEngine
        src/main.cpp
        src/movegen/movegen.h
        src/movegen/movegen.cpp
        src/board.cpp
        src/movegen/update.cpp
        src/init.cpp
        src/misc.cpp
        include/misc.h

        src/search/search.cpp
        include/macros.h

        src/UCI.cpp
        src/eval/evaluation.h

        src/hashtable.cpp
        src/search/search.h
        include/uci.h
        include/hashtable.h
        include/init.h
        include/board.h
        src/movegen/update.h
        include/inline_functions.h
        src/eval/evaluation.cpp

        src/search/movesort.cpp
        src/search/movesort.h
        include/types.h

        ## Needed for the engine to play
        benchmark_tests/benchmark_tests.h
        benchmark_tests/perft_tests.cpp
        benchmark_tests/branching_factor.cpp

        ## Needed for testing
        benchmark_tests/puzzle_tests.cpp

        debug_tests/evaluation_tests.cpp
        debug_tests/debug_tests.h
        debug_tests/move_tests.cpp
        debug_tests/bitmask_tests.cpp

        ## Needed for tuning

)
