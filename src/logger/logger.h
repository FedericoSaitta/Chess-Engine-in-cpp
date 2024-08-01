#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <filesystem>  // C++17

class Logger {
private:
    std::string logFileName;

    // Function to get the current timestamp
    static std::string getCurrentTime() {
        const std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buf;
    }

    // Function to create the log file if it doesn't exist
    void createLogFileIfNotExists() const {
        // Check if the file exists using std::filesystem
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
    explicit Logger(const std::string& filename) : logFileName(filename) {
        // Ensure the directory exists using std::filesystem
        const std::filesystem::path logPath(logFileName);
        const std::filesystem::path dirPath = logPath.parent_path();

        if (!std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) { // Create directory if it doesn't exist
                std::cerr << "Error: Could not create directories for the log file: " << logFileName << std::endl;
                return; // Early return if directory creation fails
            }
        }

        // Create the log file if it doesn't exist
        createLogFileIfNotExists();
    }

    // Function to clear the log file
    void clearLogFile() const {
        std::ofstream ofs(logFileName, std::ofstream::out | std::ofstream::trunc);
        if (!ofs.is_open()) {
            std::cerr << "Error clearing log file: " << logFileName << std::endl;
        } else {
            ofs << "[" << getCurrentTime() << "] " << "Log file cleared.\n";
            ofs.close();
        }
    }

    void log(const std::string& message) {
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

    void logError(const std::string& error) { log("ERROR: " + error); }
    void logWarning(const std::string& warning) { log("WARNING: " + warning); }
    void logInfo(const std::string& info) { log("INFO: " + info); }
};

extern Logger logFile;
