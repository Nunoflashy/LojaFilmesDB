#ifndef LOGGER_H
#define LOGGER_H

//#include "App.h"

#define LOG(msg,level) \
        Log(__func__,msg,level); \
        printf("%s() -> %s: %s", __func__, msg, LogLevelToString(level))

#define LOGF(level,fmt, ...) \
        Logf(__func__,level,fmt,__VA_ARGS__); \
        printf(fmt,__VA_ARGS__); \
        printf("\n")



typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_CRITICAL
} LogLevel;

void Log(const char* caller, const char* msg, LogLevel level);
void Logf(const char* caller, LogLevel level, const char* fmt, ...);
void LimparLog();
char* LogLevelToString();

#endif // LOGGER_H
