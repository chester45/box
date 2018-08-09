#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "log.h"

log_func_t log_func = NULL;

/**
**---------------------------------------------------------------------------
**  Abstract: Convert integer to ascii
**  Returns:  void
**---------------------------------------------------------------------------
*/
static void ts_itoa(char **buf, uint16_t d, uint16_t base)
{
    uint16_t div = 1;
    while (d / div >= base)
        div *= base;

    while (div != 0)
    {
        uint16_t num = d / div;
        d = d % div;
        div /= base;
        if (num > 9)
            *((*buf)++) = (num - 10) + 'A';
        else
            *((*buf)++) = num + '0';
    }
}

/**
**---------------------------------------------------------------------------
**  Abstract: Writes arguments va to buffer buf according to format fmt
**  Returns:  Length of string
**---------------------------------------------------------------------------
*/
static uint16_t ts_formatstring(char *buf, const char *fmt, va_list va)
{
    char *start_buf = buf;
    while (*fmt)
    {
        /* Character needs formating? */
        if (*fmt == '%')
        {
            switch (*(++fmt))
            {
            case 'c':
                *buf++ = va_arg(va, uint16_t);
                break;
            case 'd':
            case 'i':
            {
                int16_t val = va_arg(va, int16_t);
                if (val < 0)
                {
                    val *= -1;
                    *buf++ = '-';
                }
                ts_itoa(&buf, val, 10);
            }
            break;
            case 's':
            {
                char * arg = va_arg(va, char *);
                while (*arg)
                {
                    *buf++ = *arg++;
                }
            }
            break;
            case 'u':
                ts_itoa(&buf, va_arg(va, uint16_t), 10);
                break;
            case 'x':
            case 'X':
                ts_itoa(&buf, va_arg(va, uint16_t), 16);
                break;
            case '%':
                *buf++ = '%';
                break;
            }
            fmt++;
        }
        /* Else just copy */
        else
        {
            *buf++ = *fmt++;
        }
    }
    *buf = 0;

    return (uint16_t)(buf - start_buf);
}

/**
**---------------------------------------------------------------------------
**  Abstract: Calculate maximum length of the resulting string from the
**            format string and va_list va
**  Returns:  Maximum length
**---------------------------------------------------------------------------
*/
static uint16_t ts_formatlength(const char *fmt, va_list va)
{
    uint16_t length = 0;
    while (*fmt)
    {
        if (*fmt == '%')
        {
            ++fmt;
            switch (*fmt)
            {
            case 'c':
                va_arg(va, uint16_t);
                ++length;
                break;
            case 'd':
            case 'i':
            case 'u':
                /* 32 bits integer is max 11 characters with minus sign */
                length += 11;
                va_arg(va, uint16_t);
                break;
            case 's':
            {
                char * str = va_arg(va, char *);
                while (*str++)
                    ++length;
            }
            break;
            case 'x':
            case 'X':
                /* 32 bits integer as hex is max 8 characters */
                length += 8;
                va_arg(va, uint16_t);
                break;
            default:
                ++length;
                break;
            }
        }
        else
        {
            ++length;
        }
        ++fmt;
    }
    return length;
}

void log_initialize(const log_func_t func_ptr)
{
    log_func = func_ptr;
}

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            standard output according to the format parameter.
**  Returns:  Number of bytes written
**
**===========================================================================
*/
void log_printf(const char *fmt, ...)
{
    if (log_func)
    {
        uint16_t length = 0;
        va_list va;
        va_start(va, fmt);
        length = ts_formatlength(fmt, va);
        va_end(va);
        {
            char buf[length];
            va_start(va, fmt);
            length = ts_formatstring(buf, fmt, va);
            log_func(buf);
            va_end(va);
        }
    }

}