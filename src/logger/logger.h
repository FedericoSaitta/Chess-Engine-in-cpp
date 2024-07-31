//
// Created by Federico Saitta on 31/07/2024.
//
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>

#include "types.h"
#include "misc.h"

// Include filesystem if you're using C++17 or later
#include <filesystem>

// info is the highest level of logging
enum LogLevel { NONE = 0, ERROR, WARNING, INFO };

class Logger {
public:
    std::string logFileName;
    int loggingLevel;

    // Function to get the current timestamp
    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buf;
    }

    // Function to create the log file if it doesn't exist
    void createLogFileIfNotExists() {
        // Check if the file exists
        std::ifstream infile(logFileName);
        if (!infile.good()) {  // File does not exist
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
    Logger(const std::string& filename, const int logLevel = NONE)
        : logFileName(filename), loggingLevel(logLevel) {

        // Ensure the directory exists (using C++17 std::filesystem)
        std::filesystem::path logPath(logFileName);
        std::filesystem::path dirPath = logPath.parent_path();

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath); // Create directory if it doesn't exist
        }

        // Create the log file if it doesn't exist
        createLogFileIfNotExists();

        // Clear the log file at startup
        clearLogFile();
    }

    // Function to clear the log file
    void clearLogFile() {
        std::ofstream ofs(logFileName, std::ofstream::out | std::ofstream::trunc);
        if (!ofs.is_open()) {
            std::cerr << "Error clearing log file: " << logFileName << std::endl;
        }
    }

    // Function to log a message
    void log(const std::string& message) {
        if (loggingLevel > 0) {
            std::ofstream ofs(logFileName, std::ofstream::out | std::ofstream::app);
            if (ofs.is_open()) {
                ofs << "[" << getCurrentTime() << "] " << message << std::endl;
            } else {
                std::cerr << "Error opening log file: " << logFileName << std::endl;
            }
        }
    }

    // Overloaded log function to accept different message types
    template <typename T>
    void log(const T& message) {
        if (loggingLevel > 0) {
            std::ostringstream oss;
            oss << message;
            log(oss.str());
        }
    }

    // Log an error message
    void logError(const std::string& error) {
        if (loggingLevel > 0) {
            log("ERROR: " + error);
        }
    }

    // Log a warning message
    void logWarning(const std::string& warning) {
        if (loggingLevel > 1) {
            log("WARNING: " + warning);
        }
    }

    // Log an info message
    void logInfo(const std::string& info) {
        if (loggingLevel > 2) {
            log("INFO: " + info);
        }
    }

    void setLoggingLevel(const LogLevel lvl) {
        loggingLevel = lvl;
    }
};

// Create a logger object with a default log file path
inline extern Logger logFile("/Users/federicosaitta/CLionProjects/ChessEngine/src/logger/logFile.txt", INFO);
