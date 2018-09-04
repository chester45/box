#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>

#define ENABLE_LOG  (1)

typedef void(*LogFunc_t)(char *);

#define DEFINE_DEBUG_TAG(debugTag) const char _debug_tag[] = debugTag

void LogInitialize(const LogFunc_t);
void LogPrintf(const char *fmt, ...);
void LogPrintfDebug(const char *tag, const char *fmt, ...);

#if ENABLE_LOG
        #define LOG_BASIC(str, ...) LogPrintf(str, ##__VA_ARGS__)
        #define LOG(str, ...) LogPrintfDebug(_debug_tag, str, ##__VA_ARGS__)
#else
        #define LOG(str, ...) ;
#endif

#endif