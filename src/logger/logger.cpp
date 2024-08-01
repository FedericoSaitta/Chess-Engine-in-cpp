#include "logger.h"
#include <filesystem>

// Helper function to get the project's directory
std::string getProjectDirectory() {
    // Get the current source directory and move one level up
    // Assumes that this file (logger.cpp) is located within the 'src' directory
    std::filesystem::path currentPath = std::filesystem::current_path();

    // Traverse upwards until the project root is found (common CMake pattern)
    while (!std::filesystem::exists(currentPath / "CMakeLists.txt") && currentPath.has_parent_path()) {
        currentPath = currentPath.parent_path();
    }

    return currentPath.string();
}

// Define the global logger with a path relative to the project's root
Logger logFile((getProjectDirectory() + "/logFile.txt"));