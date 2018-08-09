#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>

#define ENABLE_LOG  (1)

typedef void(*log_func_t)(char *);

void log_initialize(const log_func_t);
void log_printf(const char *fmt, ...);

#if ENABLE_LOG
    #define LOG(str) log_printf(str);
#else
    #define LOG(str) ;
#endif

#endif