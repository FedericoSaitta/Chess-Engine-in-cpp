#include "logger.h"
#include <filesystem>
#include <iostream>

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

// Function to get the directory of the running executable
std::string getExecutableDirectory() {
    std::string executablePath;

#ifdef _WIN32
    // Windows implementation
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    executablePath = std::string(path);
#elif __linux__
    // Linux implementation
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        executablePath = std::string(path, count);
    }
#elif __APPLE__
    // macOS implementation
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        executablePath = std::string(path);
    } else {
        std::cerr << "Error: Buffer size too small for executable path." << std::endl;
    }
#endif

    // Use std::filesystem to get the parent directory of the executable
    return std::filesystem::path(executablePath).parent_path().string();
}

bool isLoggingEnabled() {

#ifdef __APPLE__
    #ifdef RELEASE_BUILD
        return false; // Disable logging in release builds
    #else
        return true;  // Enable logging in debug builds
    #endif
#endif

    return false;


}

// Define the global logger with a path in the current executable directory
Logger logFile(
    (std::filesystem::current_path() / "logFile.txt").string(),
    isLoggingEnabled()
);