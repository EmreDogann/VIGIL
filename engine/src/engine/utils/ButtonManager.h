#pragma once
#include "game/UI/ButtonText.h"
#include "game/UI/ButtonImage.h"

class ButtonManager {
public:
	ButtonManager(const glm::mat4& view_projection_matrix);

	~ButtonManager() {
		currentActive = nullptr;
	};
	void create_text_button(std::string text, float text_size, glm::vec2 text_pos, glm::vec4 text_color, glm::vec4 text_color_active, engine::ref<engine::text_manager>& text_manager, std::function<void()> onClick);
	void create_image_button(glm::vec2             image_pos, glm::vec2 image_size, engine::ref<engine::game_object>& image, std::vector<engine::ref<engine::game_object>>& image_selected,
		std::function<void()> onClick);
	void      check_confirm();
	int       check_hover_text(float mouse_x, float mouse_y);
	int       check_hover_images(float mouse_x, float mouse_y);
	glm::vec3 screen_to_world(glm::vec3 pos);
	glm::vec2 world_to_screen(glm::vec3 pos);
	glm::vec3 screen_to_world(glm::vec2 pos);
	glm::vec2 world_to_screen(glm::vec2 pos);
	void      on_render_text(const engine::ref<engine::shader>& shader);
	void      on_render_image(const engine::ref<engine::shader>& shader);
	void      on_render_image_single(const engine::ref<engine::shader>& shader, ButtonImage image, int selected_image_index);

	std::vector<ButtonImage> get_button_images() {
		return buttons_image;
	}

	void set_active(bool active) {
		is_active = active;
	};
	static engine::ref<ButtonManager> create(const glm::mat4& view_projection_matrix);

private:
	std::vector<ButtonText>  buttons_text;
	std::vector<ButtonImage> buttons_image;
	Button* currentActive = nullptr;
	bool                     is_active = false;
	const glm::mat4& view_projection_matrix;

	engine::ref<engine::audio_manager> m_audio_manager{};
};
