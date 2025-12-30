#pragma once

#ifndef NDEBUG
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include <spdlog/spdlog.h>

namespace Kyber {

    class Logger
    {
    public:
        static auto Init() -> void;
        static auto Shutdown() -> void;

        static auto Flush() -> void;

    private:
        inline static bool s_Initialized { false };
    };

}

#define KTRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define KDEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define KINFO(...)  SPDLOG_INFO(__VA_ARGS__)
#define KWARN(...)  SPDLOG_WARN(__VA_ARGS__)
#define KERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define KFATAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
