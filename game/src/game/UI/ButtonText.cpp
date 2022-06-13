#include "pch.h"
#include "ButtonText.h"

ButtonText::ButtonText(std::string text, float text_size, glm::vec2 text_pos, glm::vec4 text_color, glm::vec4 text_color_active, engine::ref<engine::text_manager>& text_manager, std::function<void ()> onClick) :
	text(text), text_size(text_size), text_pos(text_pos), text_color(text_color), text_color_active(text_color_active), text_manager(text_manager) {
	padding *= text_size;
	this->onClick = onClick;
	text_manager->get_text_dimensions(text, text_size, dimensions);

	// Add padding
	dimensions.x -= padding;
	dimensions.y -= padding;
	dimensions.w += padding;
	dimensions.z += padding;
}

ButtonText::~ButtonText() {}

bool ButtonText::check_mouse_pos(float mouse_x, float mouse_y) {
	// Normalize mouse screen coordinates.
	mouse_x = mouse_x / (float)engine::application::window().width();
	mouse_y = mouse_y / (float)engine::application::window().height();

	//Normalize button dimensions.
	glm::vec4 dimensions_normalized{};
	dimensions_normalized.x = dimensions.x / (float)engine::application::window().width();
	dimensions_normalized.z = dimensions.z / (float)engine::application::window().width();
	dimensions_normalized.y = dimensions.y / (float)engine::application::window().height();
	dimensions_normalized.w = dimensions.w / (float)engine::application::window().height();

	if ((mouse_x >= dimensions_normalized.x && mouse_x <= dimensions_normalized.z) && (mouse_y <= 1.0f - dimensions_normalized.y && mouse_y >= 1.0f - dimensions_normalized.w)) {
		active = true;
		return true;
	}

	active = false;
	return false;
}

void ButtonText::on_render(const engine::ref<engine::shader>& shader) {
	glm::vec2 text_pos_centered = {text_pos.x - (std::abs(dimensions.z - dimensions.x) / 2.0f), text_pos.y - (std::abs(dimensions.w - dimensions.y) / 2.0f)};

	if (active) {
		text_manager->render_text(shader, text, text_pos_centered.x, text_pos_centered.y, text_size, text_color_active, dimensions);
	} else {
		text_manager->render_text(shader, text, text_pos_centered.x, text_pos_centered.y, text_size, text_color, dimensions);
	}

	// Add padding
	dimensions.x -= padding;
	dimensions.y -= padding;
	dimensions.w += padding;
	dimensions.z += padding;
}
