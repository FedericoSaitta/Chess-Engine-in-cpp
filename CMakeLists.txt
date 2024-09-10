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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /fp:fast /permissive- /W4 /std:c++latest")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG /fsanitize=address")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang specific flags for both Debug and Release
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fno-omit-frame-pointer")

    set(SANITIZER_FLAGS "-fsanitize=address,undefined,thread,leak")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 ${SANITIZER_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")

    # Linker settings for sanitizers (only applied in debug mode)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
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
        benchmark_tests/perft_tests.cpp
        benchmark_tests/search_and_eval.cpp
        benchmark_tests/puzzle_tests.cpp
        debug_tests/evaluation_tests.cpp
        debug_tests/move_tests.cpp
        debug_tests/bitmask_tests.cpp
        src/movegen/movegen.h
        src/search/search.h
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
        src/logger/logger.h
        include/config.h
        src/logger/logger.cpp
        debug_tests/game_scenario.cpp
        include/timer.h
        include/game_statistics.h
        src/search/see.cpp
        src/search/see.h
        debug_tests/see_tests.cpp
        src/search/time_manager.cpp
        src/search/search_helper.cpp
        include/move.h
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
    target_link_libraries(ChessEngine PRIVATE pthread)
endif()

# Link Threads library
target_link_libraries(ChessEngine PRIVATE Threads::Threads)
