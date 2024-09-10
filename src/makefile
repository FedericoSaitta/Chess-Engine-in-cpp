# Variables that can be overridden when calling 'make'
CXX ?= g++  # Default C++ compiler if not provided
CC ?= gcc   # Default C compiler if not provided
EXE ?= ChessEngine  # Default executable name

# Compiler flags (can be customized based on CMakeLists.txt)
CXXFLAGS = -std=c++23 -Wall -Wextra -pedantic -fno-omit-frame-pointer
LDFLAGS = -lpthread

# Source files and object files
SRC_FILES = src/main.cpp src/movegen/movegen.cpp src/board.cpp src/movegen/update.cpp \
            src/init.cpp src/misc.cpp src/search/search.cpp src/UCI.cpp \
            src/hashtable.cpp src/eval/evaluation.cpp src/search/movesort.cpp \
            benchmark_tests/perft_tests.cpp benchmark_tests/search_and_eval.cpp \
            benchmark_tests/puzzle_tests.cpp debug_tests/evaluation_tests.cpp \
            debug_tests/move_tests.cpp debug_tests/bitmask_tests.cpp \
            debug_tests/see_tests.cpp src/search/see.cpp src/search/time_manager.cpp \
            src/search/search_helper.cpp

OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Include directories (customize based on your project's structure)
INCLUDE_DIRS = -Iinclude -Ibenchmark_tests -Idebug_tests -Ieval -Isearch -Imovegen -Ilogger -Itexel-tuner -Itexel-tuner/engine

# Target rule (how to build the executable)
$(EXE): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o $(EXE)

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJ_FILES) $(EXE)

# Phony targets to avoid conflicts with files named 'clean'
.PHONY: clean
