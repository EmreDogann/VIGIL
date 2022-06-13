#pragma once
#include "application.h"

#ifdef ENGINE_PLATFORM_WIN

extern engine::application* engine::create_application();

/// \brief Adjusts the console to take 1/3rd of the monitor height.
void move_console();


int main(int argc, char** argv) {

	move_console();

	engine::logger::init();
	LOG_CORE_WARN("Initialized logger");
	LOG_INFO("Welcome to the new engine!");

	auto app = engine::create_application();
	app->run();
	delete app;
}

void move_console() {
#ifdef ENGINE_DEBUG
	auto hwnd = GetConsoleWindow();
	SetWindowPos(hwnd, HWND_TOP, 0, static_cast<int>(1080 * 1.05), 1920, static_cast<int>(1080 * 0.25), SWP_NOZORDER);
	ShowWindow(hwnd, SW_SHOW);
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
}

#else
    #error engine only supports Windows!
#endif
