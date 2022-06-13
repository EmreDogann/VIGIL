#include "pch.h"
#include "ButtonManager.h"

ButtonManager::ButtonManager(const glm::mat4& view_projection_matrix) :
	view_projection_matrix(view_projection_matrix) {
	m_audio_manager = engine::application::audio(); // Get instance of audio manager.
}

engine::ref<ButtonManager> ButtonManager::create(const glm::mat4& view_projection_matrix) {
	return std::make_shared<ButtonManager>(view_projection_matrix);
}

void ButtonManager::create_text_button(std::string text, float text_size, glm::vec2 text_pos, glm::vec4 text_color, glm::vec4 text_color_active, engine::ref<engine::text_manager>& text_manager, std::function<void()> onClick) {
	// Convert world position to screen position so the text_renderer can correctly render the text on screen.
	ButtonText button = ButtonText(text, text_size, world_to_screen(text_pos), text_color, text_color_active, text_manager, onClick);
	buttons_text.push_back(button);
}

void ButtonManager::create_image_button(glm::vec2 image_pos, glm::vec2 image_size, engine::ref<engine::game_object>& image, std::vector<engine::ref<engine::game_object>>& image_selected, std::function<void()> onClick) {
	// For image_size: Get the maximum extent of the image (in screen space) relative to the position of the center of the image.
	ButtonImage button = ButtonImage(world_to_screen(image_pos), world_to_screen(image_pos + image_size), image, image_selected, onClick);
	buttons_image.push_back(button);
}

int ButtonManager::check_hover_text(float mouse_x, float mouse_y) {
	if (is_active) {
		int counter = 0;
		int current_active_index = -1;
		// Check if mouse has hovered over any button text elements.
		for (auto& i = buttons_text.begin(); i != buttons_text.end(); ++i) {
			if (i->check_mouse_pos(mouse_x, mouse_y)) {
				currentActive = &*i;
				current_active_index = counter;
			}
			counter++;
		}

		return current_active_index;
	}
}

int ButtonManager::check_hover_images(float mouse_x, float mouse_y) {
	if (is_active) {
		int counter = 0;
		int current_active_index = -1;
		// Check if mouse has hovered over any button image elements.
		for (auto& i = buttons_image.begin(); i != buttons_image.end(); ++i) {
			if (i->check_mouse_pos(mouse_x, mouse_y)) {
				currentActive = &*i;
				current_active_index = counter;
			}
			counter++;
		}

		return current_active_index;
	}
}

glm::vec3 ButtonManager::screen_to_world(glm::vec3 pos) {
	// Compute the Normalized Device Coordinates for the mouse position.
	glm::vec3 ndc = glm::vec3(pos.x / (float)engine::application::window().width(), pos.y / (float)engine::application::window().height(),
	                          pos.z) * 2.0f - 1.0f;

	// Compute the world position from the NDC and camera view-projection matrix.
	glm::vec4 world_pos = glm::inverse(view_projection_matrix) * glm::vec4(ndc, 1);

	// Convert from homogeneous to cartesian coordinates.
	return glm::vec3(world_pos) / world_pos.w;
}

glm::vec2 ButtonManager::world_to_screen(glm::vec3 pos) {
	// Compute the world position from the NDC and camera view-projection matrix.
	glm::vec4 clip_space_pos = view_projection_matrix * glm::vec4(pos, 1.0f);
	glm::vec3 ndc_space = glm::vec3(clip_space_pos.x, clip_space_pos.y, clip_space_pos.z) / clip_space_pos.w;

	// Convert from homogeneous to cartesian coordinates.
	return ((glm::vec2(ndc_space.x, ndc_space.y) + 1.0f) / 2.0f) * glm::vec2((float)engine::application::window().width(), (float)engine::application::window().height());
}

glm::vec3 ButtonManager::screen_to_world(glm::vec2 pos) {
	return screen_to_world({pos.x, pos.y, 1.0f});
}

glm::vec2 ButtonManager::world_to_screen(glm::vec2 pos) {
	return world_to_screen({pos.x, pos.y, 1.0f});
}

// Check if a text or image button has been pressed and call it's respective onClick() function accordingly.
void ButtonManager::check_confirm() {
	if (currentActive != nullptr && currentActive->active && is_active) {
		m_audio_manager->play("ui_click");
		currentActive->onClick();
	}
}

void ButtonManager::on_render_text(const engine::ref<engine::shader>& shader) {
	for (auto i = buttons_text.begin(); i != buttons_text.end(); ++i) {
		i->on_render(shader);
	}
}

// Render all image buttons, using the first selected image in the supplied array as the active image.
void ButtonManager::on_render_image(const engine::ref<engine::shader>& shader) {
	for (auto i = buttons_image.begin(); i != buttons_image.end(); ++i) {
		i->on_render(shader);
	}
}

// Render a image button, using the selected image at the index specified as the active image.
void ButtonManager::on_render_image_single(const engine::ref<engine::shader>& shader, ButtonImage image, int selected_image_index) {
	image.on_render(shader, selected_image_index);
}
