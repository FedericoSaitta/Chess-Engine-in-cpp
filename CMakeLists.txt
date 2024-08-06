cmake_minimum_required(VERSION 3.28)
project(ChessEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)


# Compiler-specific flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2 /fp:fast /permissive- /W4 /std:c++latest")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang specific flags
    set(SANITIZER_FLAGS "-fsanitize=address,undefined,thread,leak")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic ${SANITIZER_FLAGS} -fno-omit-frame-pointer -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")

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
        texel-tuner
        texel-tuner/engine
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
        src/logger/logger.cpp
        src/search/old_search.cpp
        debug_tests/game_scenario.cpp
        texel-tuner/init_tuner.h
        texel-tuner/threadpool.cpp
        texel-tuner/tuner.cpp
        texel-tuner/base.h
        texel-tuner/config.h
        texel-tuner/init_tuner.h
        texel-tuner/engine/myeval.cpp
        texel-tuner/init_tuner.cpp
)

# Add executable
add_executable(ChessEngine ${SOURCE_FILES})

# Check build type and add a compile definition
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)  # Defines the standard NDEBUG macro for release
    add_compile_definitions(RELEASE_BUILD)
else()
    add_compile_definitions(DEBUG_BUILD)
endif()

# Linker settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(ChessEngine pthread)
endif()

target_link_libraries(ChessEngine PRIVATE Threads::Threads)
