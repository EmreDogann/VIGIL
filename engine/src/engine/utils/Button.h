#pragma once
#include <engine.h>

class Button {
public:
	std::function<void()> onClick; // Callback function to invoke on button click.

	virtual bool check_mouse_pos(float mouse_x, float mouse_y) = 0;
	virtual void on_render(const engine::ref<engine::shader>& shader) = 0;

	// Is button currently active
	bool active = false;
};
