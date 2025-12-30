#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "PathConfig.inl"

namespace Kyber {

    namespace {

        std::filesystem::path s_LogPath(PathConfig::LogDir);

    }

    auto Logger::Init() -> void
    {
        if (s_Initialized) return;

        if (!std::filesystem::exists(s_LogPath)) {
            std::filesystem::create_directory(s_LogPath);
        }

        std::string logfile = (s_LogPath / "Kyber.log").string();

        std::vector<spdlog::sink_ptr> sinks {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile, true)
        };

        for (auto& sink : sinks) {
            sink->set_pattern("[%H:%M:%S %z] [%^%l%$] [thread %t] %v");
        }

        auto logger = std::make_shared<spdlog::logger>("Kyber", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::err);

        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);

        s_Initialized = true;
    }

    auto Logger::Shutdown() -> void
    {
        if (s_Initialized) {
            spdlog::shutdown();
            s_Initialized = false;
        }
    }

    auto Logger::Flush() -> void
    {
        spdlog::default_logger()->flush();
    }

}
