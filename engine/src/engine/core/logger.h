#pragma once
#include "engine/core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace engine
{
    /// \brief Wrapper around spdlog logger.
    class ENGINE_API logger
    {
    public:
        static void init();
        static std::shared_ptr<spdlog::logger>& core_logger() { return s_core_logger; }
        static std::shared_ptr<spdlog::logger>& client_logger() { return s_client_logger; }
    private:
        static std::shared_ptr<spdlog::logger> s_core_logger;
        static std::shared_ptr<spdlog::logger> s_client_logger;
    };
}

// Core log macros
#define LOG_CORE_TRACE(...)    ::engine::logger::core_logger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)     ::engine::logger::core_logger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)     ::engine::logger::core_logger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)    ::engine::logger::core_logger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...)    ::engine::logger::core_logger()->fatal(__VA_ARGS__)
 // Client log macros
#define LOG_TRACE(...)	      ::engine::logger::client_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...)	      ::engine::logger::client_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)	      ::engine::logger::client_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)	      ::engine::logger::client_logger()->error(__VA_ARGS__)
#define LOG_FATAL(...)	      ::engine::logger::client_logger()->fatal(__VA_ARGS__)
