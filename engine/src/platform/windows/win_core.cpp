#include "pch.h"
//#include "engine/core.h"
//#include "GLFW/glfw3.h"
//#include "win_input.h"
//#include "engine/logger.h"
//
//static bool g_glfw_nitialized = false;
//
//void engine::core::platform_startup()
//{
//	LOG_CORE_TRACE("[win_core] Platform-specific startup: Windows");
//
//	if(!g_glfw_nitialized)
//	{
//		const int32_t success = glfwInit();
//		if(!success)
//		{
//			LOG_CORE_FATAL("[win_core] Could not initialize GLFW!");
//			startup_success = false;
//		}
//		else
//			LOG_CORE_TRACE("[win_core] GLFW initialized");
//
//		g_glfw_nitialized = true;
//	}
//
//	LOG_CORE_TRACE("[win_core] Starting input manager");
//	
//}
//
//void engine::core::platform_shutdown()
//{
//	LOG_CORE_TRACE("[win_core] Platform-specific shutdown: Windows");
//
//	LOG_CORE_TRACE("[win_core] Stopping input manager");
//	win_input::shutdown();
//
//	if(g_glfw_nitialized)
//	{
//		glfwTerminate();
//		LOG_CORE_TRACE("[win_core] GLFW terminated");
//	}
//}
//
