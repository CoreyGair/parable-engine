#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>

namespace Parable{

class Log{
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; };
        inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; };

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;       
        static std::shared_ptr<spdlog::logger> s_ClientLogger;       
};

// Core log macros
#define PBL_CORE_TRACE(...)    ::Parable::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PBL_CORE_INFO(...)     ::Parable::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PBL_CORE_WARN(...)     ::Parable::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PBL_CORE_ERROR(...)    ::Parable::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PBL_CORE_CRITICAL(...) ::Parable::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define PBL_TRACE(...)         ::Parable::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PBL_INFO(...)          ::Parable::Log::GetClientLogger()->info(__VA_ARGS__)
#define PBL_WARN(...)          ::Parable::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PBL_ERROR(...)         ::Parable::Log::GetClientLogger()->error(__VA_ARGS__)
#define PBL_CRITICAL(...)      ::Parable::Log::GetClientLogger()->critical(__VA_ARGS__)

}