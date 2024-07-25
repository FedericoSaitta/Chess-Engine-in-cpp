cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2 /fp:fast /permissive- /W4 /std:c++latest")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")
else()
    # GCC/Clang specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")
endif()

include_directories(include)
include_directories(benchmark_tests)
include_directories(debug_tests)
include_directories(eval)
include_directories(search)
include_directories(movegen)

add_executable(ChessEngine
        # Needed for the engine to play
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

        benchmark_tests/benchmark_tests.h
        benchmark_tests/perft_tests.cpp
        benchmark_tests/branching_factor.cpp

        # Needed for testing
        benchmark_tests/puzzle_tests.cpp
        debug_tests/evaluation_tests.cpp
        debug_tests/debug_tests.h
        debug_tests/move_tests.cpp
        debug_tests/bitmask_tests.cpp

        # Needed for tuning
)
