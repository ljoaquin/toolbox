#include "debug_util.h"
#include <stdarg.h>
#include <stdio.h>

namespace toolbox
{
    static void _log(const char *format, va_list args)
    {
        // printf(format, args);
        const int buf_size = 1024;
        char buf[buf_size];
        vsnprintf(buf, buf_size, format, args);
        buf[buf_size - 1] = 0;
        puts(buf);
    }

    void log(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        _log(format, args);
        va_end(args);
    }

}
