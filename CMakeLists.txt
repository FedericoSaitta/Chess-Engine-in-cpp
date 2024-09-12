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
    # GCC/Clang specific flags
  #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fno-omit-frame-pointer")

    # Sanitizer flags for debug builds
 #   set(SANITIZER_FLAGS "-fsanitize=address,undefined,leak")  # Use ThreadSanitizer only when needed (it's slower)

    # Enable sanitizers in debug builds
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 ${SANITIZER_FLAGS}")

    # Linker settings for sanitizers
  #  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mtune=native -funroll-loops -ffast-math -flto")
endif()

# Include directories
include_directories(
        include
        chess
        engine
        test/benchmark_tests
        test/debug_tests
        eval
        search
        movegen
        logger
        texel-tuner
        texel-tuner/engine
)

# Source files
set(SOURCE_FILES
        engine/main.cpp
        chess/movegen/movegen.cpp
        chess/board.cpp
        chess/movegen/update.cpp
        engine/init.cpp
        chess/misc.cpp
        engine/search/search.cpp
        engine/UCI.cpp
        engine/hashtable.cpp
        engine/eval/evaluation.cpp
        engine/search/movesort.cpp
        test/benchmark_tests/perft_tests.cpp
        test/benchmark_tests/search_and_eval.cpp
        test/benchmark_tests/puzzle_tests.cpp
        test/debug_tests/evaluation_tests.cpp
        test/debug_tests/move_tests.cpp
        test/debug_tests/bitmask_tests.cpp
        chess/movegen/movegen.h
        engine/search/search.h
        chess/misc.h
        chess/macros.h
        include/uci.h
        include/hashtable.h
        include/init.h
        chess/board.h
        chess/inline_functions.h
        chess/types.h
        engine/eval/evaluation.h
        test/benchmark_tests/benchmark_tests.h
        test/debug_tests/debug_tests.h
        engine/logger/logger.h
        include/config.h
        engine/logger/logger.cpp
        test/debug_tests/game_scenario.cpp
        include/timer.h
        include/game_statistics.h
        chess/see.cpp
        chess/see.h
        test/debug_tests/see_tests.cpp
        engine/search/time_manager.cpp
        engine/search/search_helper.cpp
        chess/move.h
        chess/bit_operations.h
        chess/move.cpp
        chess/move.cpp
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
