#ifndef APP_H
#define APP_H

#include "MessageInfoLevel.h"

// Compatibilidade Windows
#ifdef _WIN32
    #define clear cls
#endif

//#define DEBUG

#ifdef DEBUG
    #define DEBUG_LOG(fmt, ...) Logf(__func__,LOG_DEBUG,fmt,__VA_ARGS__)
#else
    #define DEBUG_LOG
#endif

#endif // APP_H
