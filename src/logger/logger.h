#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <filesystem>

class Logger {
private:
    std::string logFileName;
    bool enabled;

    // Function to get the current timestamp
    std::string getCurrentTime() const {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buf;
    }

    // Function to create the log file if it doesn't exist
    void createLogFileIfNotExists() {
        if (!enabled) return; // Skip if logging is disabled

        if (!std::filesystem::exists(logFileName)) {  // File does not exist
            std::ofstream ofs(logFileName);
            if (ofs.is_open()) {
                ofs << "[" << getCurrentTime() << "] " << "Log file created.\n";
                ofs.close();
            } else {
                std::cerr << "Error: Could not create log file: " << logFileName << std::endl;
            }
        }
    }

public:
    // Constructor: Initializes the logger with a specified filename
    Logger(const std::string& filename, bool enableLogging = true)
        : logFileName(filename), enabled(enableLogging) {
        if (!enabled) return; // Skip initialization if logging is disabled

        std::cout << "Log File at: " << filename << std::endl;
        // Ensure the directory exists using std::filesystem
        std::filesystem::path logPath(logFileName);
        std::filesystem::path dirPath = logPath.parent_path();

        if (!std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) { // Create directory if it doesn't exist
                std::cerr << "Error: Could not create directories for the log file: " << logFileName << std::endl;
                return; // Early return if directory creation fails
            }
        }

        // Create the log file if it doesn't exist
        createLogFileIfNotExists();
        clearLogFile();
    }

    // Function to clear the log file
    void clearLogFile() {
        if (!enabled) return; // Skip if logging is disabled

        std::ofstream ofs(logFileName, std::ofstream::out | std::ofstream::trunc);
        if (!ofs.is_open()) {
            std::cerr << "Error clearing log file: " << logFileName << std::endl;
        } else {
            ofs << "[" << getCurrentTime() << "] " << "Log file cleared.\n";
            ofs.close();
        }
    }


    void log(const std::string& message) {
        if (!enabled) return; // Skip if logging is disabled

        std::ofstream ofs(logFileName, std::ofstream::out | std::ofstream::app);
        if (ofs.is_open()) {
            ofs << "[" << getCurrentTime() << "] " << message << std::endl;
            ofs.close();
        } else {
            std::cerr << "Error opening log file: " << logFileName << std::endl;
        }
    }


    template <typename T>
    void log(const T& message) {
        std::ostringstream oss;
        oss << message;
        log(oss.str());
    }

    // Log an error message
    void logError(const std::string& error) {
        if (error != "") log("ERROR: " + error);
    }

    // Log a warning message
    void logWarning(const std::string& warning) {
        if (warning != "") {
            std::cout << warning << '\n';
            log("WARNING: " + warning);
        }
    }

    void logInfo(const std::string& info) {
        if (info != "") log("INFO: " + info);
    }
};

// Declare the global logger object
extern Logger logFile;
