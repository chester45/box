#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>

#define ENABLE_LOG  (1)

typedef void(*log_func_t)(char *);

#define DEFINE_DEBUG_TAG(debugTag) const char _debug_tag[] = debugTag

void log_initialize(const log_func_t);
void log_printf(const char *fmt, ...);
void log_printf_debug(const char *tag, const char *fmt, ...);

#if ENABLE_LOG
    #define LOG_BASIC(str, ...) log_printf(str, ##__VA_ARGS__)
    #define LOG(str, ...) log_printf_debug(_debug_tag, str, ##__VA_ARGS__)
#else
    #define LOG(str, ...) ;
#endif

#endif