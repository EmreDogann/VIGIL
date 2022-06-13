#pragma once
#ifndef FIXEDCAMERA_H
#define FIXEDCAMERA_H
#include "camera.h"
#include "engine/entities/bounding_box.h"

namespace engine {
	struct fixed_camera_position {
		fixed_camera_position(): fixed_camera_position(glm::vec3(0), glm::vec3(0), false, std::make_shared<bounding_box>()) {}
		fixed_camera_position(glm::vec3 camera_position, glm::vec3 target_position, bool look_at_player, std::shared_ptr<bounding_box> trigger);
		glm::vec3                     cam_position;
		glm::vec3                     target_position;
		bool                          look_at_player;
		std::shared_ptr<bounding_box> trigger_volume;

		fixed_camera_position& operator=(const fixed_camera_position&) = default;
	};

	class fixed_camera : public perspective_camera {
	public:
		fixed_camera(float width, float height, std::shared_ptr<fixed_camera_position> starting_position, float fov = 45.f, float near_z = 0.1f,
		             float far_z = 100.f);
		~fixed_camera();

		void on_update(const timestep& timestep, const bounding_box player_bounding_box);
		void on_render(const engine::ref<engine::shader>& shader);
		void on_event(engine::event& event, engine::perspective_camera& m_3d_camera_debug, engine::game_object& m_player);

		void add_camera_position(const std::shared_ptr<fixed_camera_position>& cam_angle) {
			camera_angles.push_back(cam_angle);
		}

	private:
		void        checkTrigger(const bounding_box player_bounding_box);
		std::string vec3_to_string(const glm::vec3 pos) const;

		std::vector<std::shared_ptr<fixed_camera_position>> camera_angles;

		std::shared_ptr<fixed_camera_position> current_camera_position;
		std::shared_ptr<fixed_camera_position> debug_selected_camera; // For use in creating new camera positions only.
	};
}
#endif
