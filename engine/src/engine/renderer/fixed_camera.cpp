#include "pch.h"
#include "fixed_camera.h"

#include "engine/events/key_event.h"

// Basic file operations
#include <fstream>

engine::fixed_camera_position::fixed_camera_position(glm::vec3                     camera_position, glm::vec3 target_position, bool look_at_player,
                                                     std::shared_ptr<bounding_box> trigger) :
	cam_position(camera_position), target_position(target_position),
	look_at_player(look_at_player),
	trigger_volume(trigger) {}

engine::fixed_camera::fixed_camera(float width, float height, struct std::shared_ptr<engine::fixed_camera_position> starting_position, float fov, float near_z,
                                   float far_z):
	perspective_camera(
		width, height, fov, near_z, far_z), current_camera_position(starting_position) {}

engine::fixed_camera::~fixed_camera() = default;

void engine::fixed_camera::on_update(const engine::timestep& timestep, const bounding_box player_bounding_box) {
	checkTrigger(player_bounding_box);

	this->set_view_matrix(current_camera_position->cam_position, current_camera_position->target_position);
}

// Render the bounding-boxes of the fixed camera's trigger volumes.
void engine::fixed_camera::on_render(const engine::ref<engine::shader>& shader) {
	for (auto cam : camera_angles) {

		struct {
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;
		}         color;

		if (cam == debug_selected_camera) {
			color.g = 256.0f;
		} else {
			if (cam->look_at_player) {
				color.b = 256.0f;
			} else {
				color.r = 2.5f;
			}
		}
		cam->trigger_volume->on_render(color.r, color.g, color.b, shader);
	}
}

// Check if a trigger volume has been entered.
void engine::fixed_camera::checkTrigger(const bounding_box player_bounding_box) {
	for (auto& camera_angle : camera_angles) {
		if (camera_angle->trigger_volume->collision(player_bounding_box)) {
			current_camera_position = camera_angle;
			//position(camera_angle.cam_position);

			// If current camera position is set to look at the player, update the target position with the player's position.
			if (current_camera_position->look_at_player) {
				current_camera_position->target_position = player_bounding_box.get_pos() + glm::vec3(0.0f, 0.7f, 0.0f);
			}
		}
	}
}

struct bounding_box_info {
	bounding_box_info(float width = 1.0f, float height = 1.0f, float depth = 1.0f, glm::vec3 position = glm::vec3(1.0f)) :
		width(width), height(height), depth(depth),
		position(position) {}

	float     width;
	float     height;
	float     depth;
	glm::vec3 position;
};

void engine::fixed_camera::on_event(engine::event& event, engine::perspective_camera& m_3d_camera_debug, engine::game_object& m_player) {
	// Controls to create camera angles.
	if (event.event_type() == engine::event_type_e::key_pressed) {
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);

		if (e.key_code() == engine::key_codes::KEY_ENTER) {
			// Create Camera.
			auto cam = std::make_shared<engine::fixed_camera_position>(m_3d_camera_debug.position(),
			                                                           m_3d_camera_debug.position() + m_3d_camera_debug.front_vector(), false,
			                                                           std::make_shared<engine::bounding_box>(3.5f, 0.8f, 2.0f, m_3d_camera_debug.position()));
			camera_angles.push_back(cam);
			debug_selected_camera = cam;
		} else if (e.key_code() == engine::key_codes::KEY_BACKSLASH) {
			// Create Camera looking at player.
			auto cam = std::make_shared<engine::fixed_camera_position>(m_3d_camera_debug.position(), m_player.position(), true,
			                                                           std::make_shared<engine::bounding_box>(3.5f, 0.8f, 2.0f, m_3d_camera_debug.position()));
			camera_angles.push_back(cam);
			debug_selected_camera = cam;
		} else if (e.key_code() == engine::key_codes::KEY_BACKSPACE) {
			// Find and Delete camera position.
			auto it = camera_angles.begin();
			while (it != camera_angles.end()) {
				if ((*it)->trigger_volume->collision(engine::bounding_box(0.1f, 0.1f, 0.1f, m_3d_camera_debug.position()))) {
					if (*it == debug_selected_camera) {
						it = camera_angles.erase(it);
						if (it == camera_angles.end()) {
							debug_selected_camera = nullptr;
						} else {
							debug_selected_camera = *it;
						}
					} else {
						it = camera_angles.erase(it);
					}
				} else {
					++it;
				}
			}
		} else if (e.key_code() == engine::key_codes::KEY_RIGHT_SHIFT) {
			// Select colliding camera.
			for (auto cam : camera_angles) {
				if (cam->trigger_volume->collision(engine::bounding_box(0.1f, 0.1f, 0.1f, m_3d_camera_debug.position()))) {
					debug_selected_camera = cam;
				}
			}
		} else if (e.key_code() == engine::key_codes::KEY_4) {
			// Save camera info to file.
			bounding_box_info b_box_info{};
			std::ofstream     file;
			file.open("assets/fixed_camera_angles_level2.txt", std::ofstream::trunc);

			for (auto cam : camera_angles) {
				cam->trigger_volume->get(b_box_info.position, b_box_info.width, b_box_info.height, b_box_info.depth);

				file << vec3_to_string(cam->cam_position) << vec3_to_string(cam->target_position) << '(' << std::to_string(static_cast<int>(cam->look_at_player))
					<< "{" <<
					b_box_info.width << ','
					<< b_box_info.height << ',' <<
					b_box_info.depth << vec3_to_string(b_box_info.position) << "\n";
			}
			file.close();
		} else if (debug_selected_camera) {
			// If a camera is selected
			bounding_box_info b_box_info{};
			debug_selected_camera->trigger_volume->get(b_box_info.position, b_box_info.width, b_box_info.height, b_box_info.depth);
			b_box_info.position += glm::vec3(b_box_info.width / 2.f, 0.0f, b_box_info.depth / 2.f);
			if (e.key_code() == engine::key_codes::KEY_P) {
				// Position camera angle.
				debug_selected_camera->cam_position = m_3d_camera_debug.position();
				debug_selected_camera->target_position = m_3d_camera_debug.position() + m_3d_camera_debug.front_vector();
			} else if (e.key_code() == engine::key_codes::KEY_UP) {
				// Move Up
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.0f, 0.1f, 0.0f));
			} else if (e.key_code() == engine::key_codes::KEY_DOWN) {
				// Move Down
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.0f, -0.1f, 0.0f));
			} else if (e.key_code() == engine::key_codes::KEY_LEFT) {
				// Move Left
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(-0.1f, 0.0f, 0.0f));
			} else if (e.key_code() == engine::key_codes::KEY_RIGHT) {
				// Move Right
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.1f, 0.0f, 0.0f));
			} else if (e.key_code() == engine::key_codes::KEY_EQUAL) {
				// Move Forward
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.0f, 0.0f, 0.1f));
			} else if (e.key_code() == engine::key_codes::KEY_MINUS) {
				// Move Backward
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.0f, 0.0f, -0.1f));
			} else if (e.key_code() == engine::key_codes::KEY_F1) {
				// Increase Width
				debug_selected_camera->trigger_volume->set_box(b_box_info.width + 0.1f, b_box_info.height, b_box_info.depth, b_box_info.position);
			} else if (e.key_code() == engine::key_codes::KEY_F2) {
				// Increase Height
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height + 0.1f, b_box_info.depth, b_box_info.position);
			} else if (e.key_code() == engine::key_codes::KEY_F3) {
				// Increase Depth
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth + 0.1f, b_box_info.position);
			} else if (e.key_code() == engine::key_codes::KEY_1) {
				// Decrease Width
				debug_selected_camera->trigger_volume->set_box(b_box_info.width - 0.1f, b_box_info.height, b_box_info.depth, b_box_info.position);
			} else if (e.key_code() == engine::key_codes::KEY_2) {
				// Decrease Height
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height - 0.1f, b_box_info.depth, b_box_info.position);
			} else if (e.key_code() == engine::key_codes::KEY_3) {
				// Decrease Depth
				debug_selected_camera->trigger_volume->set_box(b_box_info.width, b_box_info.height, b_box_info.depth - 0.1f, b_box_info.position);
			}
		}
	}
}

std::string engine::fixed_camera::vec3_to_string(const glm::vec3 pos) const {
	return '(' + std::to_string(pos.x) + ',' + std::to_string(pos.y) + ',' + std::to_string(
		pos.z);
}
