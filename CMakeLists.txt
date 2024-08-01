cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2 /fp:fast /permissive- /W4 /std:c++latest")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")
endif()

# Include directories
include_directories(
        include
        benchmark_tests
        debug_tests
        eval
        search
        movegen
        logger
)

# Source files
set(SOURCE_FILES
        # Needed for the engine to play
        src/main.cpp
        src/movegen/movegen.cpp
        src/board.cpp
        src/movegen/update.cpp
        src/init.cpp
        src/misc.cpp
        src/search/search.cpp
        src/UCI.cpp
        src/hashtable.cpp
        src/eval/evaluation.cpp
        src/search/movesort.cpp
        src/search/movesort.cpp

        # Needed for testing
        benchmark_tests/perft_tests.cpp
        benchmark_tests/branching_factor.cpp
        benchmark_tests/puzzle_tests.cpp
        debug_tests/evaluation_tests.cpp
        debug_tests/move_tests.cpp
        debug_tests/bitmask_tests.cpp

        # Include header files for better IDE support
        src/movegen/movegen.h
        src/search/search.h
        src/movegen/update.h
        include/misc.h
        include/macros.h
        include/uci.h
        include/hashtable.h
        include/init.h
        include/board.h
        include/inline_functions.h
        include/types.h
        src/eval/evaluation.h

        benchmark_tests/benchmark_tests.h
        debug_tests/debug_tests.h
        src/search/searchparams.h
        src/logger/logger.h
        include/config.h
)

# Add executable
add_executable(ChessEngine ${SOURCE_FILES})

# Linker settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(ChessEngine pthread)
endif()
