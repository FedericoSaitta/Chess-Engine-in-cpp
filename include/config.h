#pragma once

#include "../src/logger/logger.h"

#define LOGGING_LEVEL_ERROR
#define LOGGING_LEVEL_WARNING
#define LOGGING_LEVEL_INFO

// In debug mode logging file is always created
#ifdef DEBUG_BUILD

        #if defined(LOGGING_LEVEL_INFO)
                #define LOG_INFO(x) logFile.logInfo(x)
                #define LOG_WARNING(x) logFile.logWarning(x)
                #define LOG_ERROR(x) logFile.logError(x)

        #elif defined(LOGGING_LEVEL_WARNING)
                #define LOG_ERROR(x) logFile.logError(x)
                #define LOG_WARNING(x) logFile.logWarning(x)
                #define LOG_INFO(x)

        #elif defined(LOGGING_LEVEL_ERROR)
                #define LOG_ERROR(x) logFile.logError(x)
                #define LOG_WARNING(x)
                #define LOG_INFO(x)
        #endif

#else

        #define LOG_ERROR(x)
        #define LOG_WARNING(x)
        #define LOG_INFO(x)
#endif
