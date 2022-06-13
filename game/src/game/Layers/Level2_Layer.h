#pragma once
#include <engine.h>
#include "game/Player/Player.h"
#include "game/AI/Pathfinding.h"
#include <random>

#include "game/AI/Enemy.h"

struct Room {
	// first: game object	 second: pair(rotation in radians, rotation axis)
	std::vector<std::pair<engine::ref<engine::game_object>, std::pair<float, glm::vec3>>> m_game_objects;
	std::vector<engine::ref<Enemy>>                                                       m_enemies;
	glm::vec3                                                                             m_last_player_pos;
	std::vector<std::pair<engine::ref<engine::bounding_box>, int>>                        m_trigger; // Pair -> 0: trigger, 1: room index to switch to.
};

class Level2_layer : public engine::layer {
public:
	Level2_layer();
	~Level2_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void detach_scene_transition(const engine::timestep& time_step) override;
	void attach_scene_transition(const engine::timestep& time_step) override;
	void set_default_shader_parameters() override;

private:
	engine::ref<engine::game_object> m_terrain{};
	engine::ref<engine::game_object> m_house{};
	engine::ref<Player>              m_player{};
	engine::ref<engine::game_object> m_first_aid_spray{};
	engine::ref<engine::game_object> m_ammo_box{};

	engine::ref<engine::material> m_material{};

	std::vector<engine::ref<engine::bounding_box>> m_obstacles;
	engine::ref<engine::bounding_box>              m_debug_selected_obstacle;

	// Stores different First Aid Spray positions.
	std::vector<glm::vec3> m_first_aid_spray_data;

	// Stores different Ammo Box positions.
	std::vector<glm::vec3> m_ammo_box_data;

	std::vector<engine::PointLight> m_point_lights;

	std::vector<Room> m_rooms;
	int               m_current_room_index = 0;
	int               m_next_room_index = m_current_room_index;

	std::vector<engine::ref<engine::game_object>> m_game_objects{};
	engine::ref<engine::game_object>              transition_image{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>   m_text_manager{};
	engine::ref<Pathfinding>            m_pathfinding{};

	engine::orthographic_camera m_2d_camera;
	engine::fixed_camera        m_3d_camera;
	engine::perspective_camera  m_3d_camera_debug;

	struct bounding_box_info {
		bounding_box_info(float width = 1.0f, float height = 1.0f, float depth = 1.0f, glm::vec3 position = glm::vec3(1.0f)) :
			width(width), height(height), depth(depth),
			position(position) {}

		float     width;
		float     height;
		float     depth;
		glm::vec3 position;
	};

	std::random_device rd;
	std::mt19937       mt = std::mt19937(rd());

	float                              m_footstep_timer_cooldown = 0.75f;
	float                              m_footstep_timer = m_footstep_timer_cooldown;
	std::uniform_int_distribution<int> m_random_footstep_sound = std::uniform_int_distribution<int>(1, 4);

	bool debug_camera = false;

	// Solve astar every 0.1 seconds.
	float                  m_pathfinding_update_frequency = 0.1f;
	float                  m_pathfinding_timer = m_pathfinding_update_frequency;
	std::vector<glm::vec3> m_current_astar_path{};

	// How long should the transition last?
	float transition_length = 2.0f;
	// A running counter of how much time if left in the transition.
	float                   transition_timer = transition_length;
	float                   m_transparency = 1.f;
	std::function<layer*()> m_next_layer;

	bool m_show_bounding_boxes = false;
	bool m_show_pathfinding = false;
};
