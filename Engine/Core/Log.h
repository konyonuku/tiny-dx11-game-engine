#pragma once
#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace Core
{
    inline void LogInfo(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::fputs("[info] ", stdout);
        std::vfprintf(stdout, fmt, args);
        std::fputc('\n', stdout);
        va_end(args);
    }

    inline void LogError(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::fputs("[error] ", stderr);
        std::vfprintf(stderr, fmt, args);
        std::fputc('\n', stderr);
        va_end(args);
    }

    inline void LogHrFailure(const char* expr, HRESULT hr, const char* file, int line)
    {
        const char* slash = std::strrchr(file, '\\');
        LogError("%s failed. hr=0x%08X (%s:%d)", expr, static_cast<unsigned>(hr),
                 slash ? slash + 1 : file, line);
    }
}

#define HR_CHECK(expr)                                                  \
    do {                                                                \
        HRESULT hr_ = (expr);                                           \
        if (FAILED(hr_)) {                                              \
            ::Core::LogHrFailure(#expr, hr_, __FILE__, __LINE__);       \
            return false;                                               \
        }                                                               \
    } while (0)
