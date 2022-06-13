#pragma once
#include "engine/utils/Button.h"

class ButtonImage : public Button {
public:
	ButtonImage(glm::vec2             image_pos, glm::vec2 image_size, engine::ref<engine::game_object>& image, std::vector<engine::ref<engine::game_object>>& image_selected,
	            std::function<void()> onClick);
	~ButtonImage();
	bool check_mouse_pos(float mouse_x, float mouse_y) override;
	void on_render(const engine::ref<engine::shader>& shader) override;
	void on_render(const engine::ref<engine::shader>& shader, int selected_image_index);

	void set_image_size(glm::vec2 size) {
		image_size = size;
	}

	glm::vec2 get_image_size() {
		return image_size;
	}

private:
	// Width and Height of the image in screen space.
	glm::vec2 image_size = glm::vec2(1.0f);

	// Padding in screen space (i.e. pixels).
	float padding = 0.f;

	// Position in screen coordinates.
	glm::vec2 image_pos = {0.f, 0.f};

	engine::ref<engine::game_object>              image;
	std::vector<engine::ref<engine::game_object>> images_selected;
};
