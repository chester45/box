#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#include <Arduino.h>

#define ENABLE_LOG  (1)

typedef void(*LogFunc_t)(char *);

#define DEFINE_DEBUG_TAG(debugTag) const char _debug_tag[] PROGMEM = debugTag

void LogPrintf(const char *fmt, ...);
void LogPrintf_PMEM(const __FlashStringHelper *str);
void LogPrintfDebug(const char *tag, const char *fmt, ...);
void LogPrintDebug_PMEM(const __FlashStringHelper *tag, const __FlashStringHelper *str);

#if ENABLE_LOG
        #define LOG_BASIC(str, ...)     LogPrintf(str, ##__VA_ARGS__)
        #define LOG_BASIC_PM(str)       LogPrintf_PMEM(F(str))
        #define LOG(str, ...)           LogPrintfDebug(_debug_tag, str, ##__VA_ARGS__)
        #define LOG_PM(str)             LogPrintDebug_PMEM((__FlashStringHelper*)_debug_tag , F(str))
#else
        #define LOG(str, ...) ;
#endif

#endif