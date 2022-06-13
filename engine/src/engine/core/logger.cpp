#include "pch.h"
#include "logger.h"

std::shared_ptr<spdlog::logger> engine::logger::s_core_logger;
std::shared_ptr<spdlog::logger> engine::logger::s_client_logger;

void engine::logger::init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_core_logger = spdlog::stdout_color_mt("ENGINE");
    s_core_logger->set_level(spdlog::level::trace);
    s_client_logger = spdlog::stdout_color_mt("GAME");
    s_client_logger->set_level(spdlog::level::trace);
}
