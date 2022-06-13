#include "pch.h"
#include "ButtonImage.h"

ButtonImage::ButtonImage(glm::vec2 image_pos, glm::vec2 image_size, engine::ref<engine::game_object>& image, std::vector<engine::ref<engine::game_object>>& image_selected, std::function<void()> onClick) :
	image_pos(image_pos)
	, image_size(image_size)
	, image(image)
	, images_selected(image_selected) {
	this->onClick = onClick;
	this->image_size += padding;

	// Normalize screen coordinates.
	this->image_size.x /= (float)engine::application::window().width();
	this->image_size.y /= (float)engine::application::window().height();
	this->image_pos.x /= (float)engine::application::window().width();
	this->image_pos.y /= (float)engine::application::window().height();

	// Get the absolute difference between the center of the image and the maximum extents of the image.
	// This is the size offset which will be used for mouse 2D Box collision.
	this->image_size.x = std::abs(this->image_size.x - this->image_pos.x);
	this->image_size.y = std::abs(this->image_size.y - this->image_pos.y);
}

ButtonImage::~ButtonImage() {}

bool ButtonImage::check_mouse_pos(float mouse_x, float mouse_y) {
	// Normalize mouse screen coordinates.
	mouse_x = mouse_x / (float)engine::application::window().width();
	mouse_y = mouse_y / (float)engine::application::window().height();
	mouse_y = 1.0f - mouse_y; // Flip mouse y coordinate so it is relative to bottom-right instead of top-left.

	//if ((mouse_x >= dimensions.x && mouse_x <= dimensions.z) && (mouse_y >= dimensions.y && mouse_y <= dimensions.w)) {	// For use if mouse & dimensions are in world space.
	if ((mouse_x >= (image_pos.x - image_size.x) && mouse_x <= (image_pos.x + image_size.x)) && (mouse_y <= image_pos.y + image_size.y && mouse_y >= image_pos.y - image_size.y)) {
		active = true;
		return true;
	}

	active = false;
	return false;
}

void ButtonImage::on_render(const engine::ref<engine::shader>& shader, int selected_image_index) {
	if (active) {
		glm::mat4 inventory_slot_selected_transform(1.0f);
		inventory_slot_selected_transform = glm::translate(inventory_slot_selected_transform, image->position());
		inventory_slot_selected_transform = glm::rotate(inventory_slot_selected_transform, image->rotation_amount(), image->rotation_axis());
		inventory_slot_selected_transform = glm::scale(inventory_slot_selected_transform, image->scale());

		engine::renderer::submit(shader, inventory_slot_selected_transform, images_selected[selected_image_index]);
	} else {
		glm::mat4 inventory_slot_transform(1.0f);
		inventory_slot_transform = glm::translate(inventory_slot_transform, image->position());
		inventory_slot_transform = glm::rotate(inventory_slot_transform, image->rotation_amount(), image->rotation_axis());
		inventory_slot_transform = glm::scale(inventory_slot_transform, image->scale());

		engine::renderer::submit(shader, inventory_slot_transform, image);
	}
}

void ButtonImage::on_render(const engine::ref<engine::shader>& shader) {
	if (active) {
		glm::mat4 inventory_slot_selected_transform(1.0f);
		inventory_slot_selected_transform = glm::translate(inventory_slot_selected_transform, image->position());
		inventory_slot_selected_transform = glm::rotate(inventory_slot_selected_transform, image->rotation_amount(), image->rotation_axis());
		inventory_slot_selected_transform = glm::scale(inventory_slot_selected_transform, image->scale());

		engine::renderer::submit(shader, inventory_slot_selected_transform, images_selected[0]);
	} else {
		glm::mat4 inventory_slot_transform(1.0f);
		inventory_slot_transform = glm::translate(inventory_slot_transform, image->position());
		inventory_slot_transform = glm::rotate(inventory_slot_transform, image->rotation_amount(), image->rotation_axis());
		inventory_slot_transform = glm::scale(inventory_slot_transform, image->scale());

		engine::renderer::submit(shader, inventory_slot_transform, image);
	}
}
