#include "Log.h"

namespace Parable
{

    std::shared_ptr<spdlog::logger> Log::s_core_logger;
    std::shared_ptr<spdlog::logger> Log::s_client_logger; 

    void Log::init()
    {
        spdlog::set_pattern("%^[%T] %n : %v%$");

        s_core_logger = spdlog::stdout_color_mt("PARABLE_ENGINE");
        s_core_logger->set_level(spdlog::level::trace);

        s_client_logger = spdlog::stdout_color_mt("PARABLE_APP");
        s_client_logger->set_level(spdlog::level::trace);
    }    
       
}