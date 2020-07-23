#include "Logger.h"
#include <stdio.h>
#include <string.h>
#include "Data.h"
#include <stdarg.h>

void Log(const char *caller, const char* msg, LogLevel level) {
    FILE *logFile = fopen("data/log.log", "a+");

    char logLevelStr[10];

    switch(level) {
        case LOG_DEBUG:     strcpy(logLevelStr, "DEBUG"); break;
        case LOG_INFO:      strcpy(logLevelStr, "INFO"); break;
        case LOG_WARN:      strcpy(logLevelStr, "AVISO"); break;
        case LOG_ERROR:     strcpy(logLevelStr, "ERRO"); break;
        case LOG_CRITICAL:  strcpy(logLevelStr, "CRITICO"); break;
    }
    //printf("Horas: %.2d:%.2d:%.2d\n", GetHora(), GetMinuto(), GetSegundo());
    fprintf(logFile, "[%s - %s] %s() -> %s: %s", Data_ToString(), Horas_ToString(), caller, logLevelStr, msg);

    fclose(logFile);
}

void Logf(const char* caller, LogLevel level, const char* fmt, ...) {
    FILE *logFile = fopen("data/log.log", "a+");

    char logLevelStr[10];
    switch(level) {
        case LOG_DEBUG:     strcpy(logLevelStr, "DEBUG"); break;
        case LOG_INFO:      strcpy(logLevelStr, "INFO"); break;
        case LOG_WARN:      strcpy(logLevelStr, "AVISO"); break;
        case LOG_ERROR:     strcpy(logLevelStr, "ERRO"); break;
        case LOG_CRITICAL:  strcpy(logLevelStr, "CRITICO"); break;
    }

    char logInfo[1000];
    snprintf(logInfo, 1000, "[%s] %s: %s():", Data_ToString(), logLevelStr, caller);

    char buffer[4096];
    snprintf(buffer, 4096, "%s %s\n", logInfo, fmt);

    va_list args;
    va_start(args, fmt);
    vfprintf(logFile, buffer, args);
    va_end(args);

    fclose(logFile);
}


void LimparLog() {
    Ficheiro_Limpar("data/log.log");
}

char* LogLevelToString(LogLevel level) {
    switch(level) {
        case LOG_INFO:      return "INFO";
        case LOG_WARN:      return "AVISO";
        case LOG_ERROR:     return "ERRO";
        case LOG_CRITICAL:  return "CRITICO";
    }
}
