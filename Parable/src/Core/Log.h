#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>

namespace Parable
{

// Engine logging header, holds logging initialisation and macros
class Log{
    public:
        static void init();
        inline static std::shared_ptr<spdlog::logger> get_core_logger() { return s_core_logger; };
        inline static std::shared_ptr<spdlog::logger> get_client_logger() { return s_client_logger; };

    private:
        static std::shared_ptr<spdlog::logger> s_core_logger;       
        static std::shared_ptr<spdlog::logger> s_client_logger;       
};

// Core log macros
#define PBL_CORE_TRACE(...)    ::Parable::Log::get_core_logger()->trace(__VA_ARGS__)
#define PBL_CORE_INFO(...)     ::Parable::Log::get_core_logger()->info(__VA_ARGS__)
#define PBL_CORE_WARN(...)     ::Parable::Log::get_core_logger()->warn(__VA_ARGS__)
#define PBL_CORE_ERROR(...)    ::Parable::Log::get_core_logger()->error(__VA_ARGS__)
#define PBL_CORE_CRITICAL(...) ::Parable::Log::get_core_logger()->critical(__VA_ARGS__)

// Client log macros
#define PBL_TRACE(...)         ::Parable::Log::get_client_logger()->trace(__VA_ARGS__)
#define PBL_INFO(...)          ::Parable::Log::get_client_logger()->info(__VA_ARGS__)
#define PBL_WARN(...)          ::Parable::Log::get_client_logger()->warn(__VA_ARGS__)
#define PBL_ERROR(...)         ::Parable::Log::get_client_logger()->error(__VA_ARGS__)
#define PBL_CRITICAL(...)      ::Parable::Log::get_client_logger()->critical(__VA_ARGS__)

}