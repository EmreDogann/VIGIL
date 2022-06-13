#pragma once
#include "engine/utils/Button.h"

class ButtonText : public Button {
public:
	ButtonText(std::string text, float text_size, glm::vec2 text_pos, glm::vec4 text_color, glm::vec4 text_color_active, engine::ref<engine::text_manager>& text_manager, std::function<void()> onClick);
	~ButtonText();
	bool check_mouse_pos(float mouse_x, float mouse_y) override;
	void on_render(const engine::ref<engine::shader>& shader) override;

	void set_dimensions(glm::vec4 val) {
		dimensions = val;
	}

	glm::vec4 get_dimensions() {
		return dimensions;
	}

private:
	// All numbers should be normalized screen coords.
	// x,y = Bottom-left Origin
	// z,w = Top-Right
	glm::vec4 dimensions = glm::vec4(-1.f);

	// Padding in screen space (i.e. pixels).
	float padding = 5.f;

	std::string text;
	float       text_size = 1.f;
	glm::vec2   text_pos = {0.f, 0.f};
	glm::vec4   text_color = {1.f, 1.f, 1.f, 0.5f};
	glm::vec4   text_color_active = {1.f, 1.f, 1.f, 1.f};

	engine::ref<engine::text_manager>& text_manager;
};
