#pragma once
#include <engine.h>
#include "game/Player/Player.h"
#include <random>

#include "engine/entities/bounding_box.h"
#include "engine/entities/shapes/heightmap.h"

class Level1_layer : public engine::layer {
public:
	Level1_layer();
	~Level1_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void detach_scene_transition(const engine::timestep& time_step) override;
	void attach_scene_transition(const engine::timestep& time_step) override;
	void set_default_shader_parameters() override;

private:
	engine::ref<engine::skybox>      m_skybox{};
	engine::ref<engine::game_object> m_terrain{};
	engine::ref<engine::heightmap>   m_heightmap{};
	engine::ref<Player>              m_player{};
	engine::ref<engine::game_object> m_sign{};
	engine::ref<engine::game_object> m_car{};
	engine::ref<engine::game_object> m_mansion{};
	engine::ref<engine::game_object> m_tree{};

	// 0: Random position, 1: Random angle-y, 2: Random texture index
	std::vector<std::tuple<glm::vec3, float, int>> m_trees_data;
	std::vector<engine::ref<engine::texture_2d>>   m_tree_textures{};

	// 0: Random position, 1: Random angle-y, 2: Random angle-z, 3: Random lifespan, 4: Random texture index
	std::vector<std::tuple<glm::vec3, float, float, int, int>> m_signs_data;
	std::vector<engine::ref<engine::texture_2d>>               m_sign_textures{};

	float sign_texture_timer = 0.f;

	// Number of trees to spawn.
	int m_tree_count = 500;

	// Stores different car positions.
	std::vector<glm::vec3> m_car_data;

	std::vector<engine::bounding_box> m_bounding_boxes;

	engine::ref<engine::material> m_material{};
	engine::ref<engine::material> m_player_material{};

	engine::SpotLight        m_spotLight;
	engine::DirectionalLight m_directionalLight;

	std::vector<engine::ref<engine::game_object>> m_game_objects{};
	engine::ref<engine::game_object>              transition_image{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager;
	engine::ref<engine::text_manager>   m_text_manager{};

	engine::orthographic_camera m_2d_camera;
	engine::fixed_camera        m_3d_camera;
	engine::perspective_camera  m_3d_camera_debug;

	bool debug_camera = false;

	std::random_device rd;
	std::mt19937       mt = std::mt19937(rd());

	//Random distributions for sign.
	std::uniform_real_distribution<float> m_dist_sign_y = std::uniform_real_distribution<float>(0.05f, 0.3f);
	std::uniform_real_distribution<float> m_dist_sign_angle_y = std::uniform_real_distribution<float>(5.f, 45.0f);
	std::uniform_real_distribution<float> m_dist_sign_angle_z = std::uniform_real_distribution<float>(-25.0f, 25.0f);
	std::uniform_real_distribution<float> m_dist_sign_lifespan = std::uniform_real_distribution<float>(5.f, 15.f);
	std::uniform_int_distribution<int>    m_dist_sign_texture_index = std::uniform_int_distribution<int>(0, 2);

	//Random distributions for tree spawns.
	std::uniform_real_distribution<float> m_dist_tree_x = std::uniform_real_distribution<float>(2.0f, 8.0f);
	std::uniform_real_distribution<float> m_dist_tree_z = std::uniform_real_distribution<float>(1.0f, 50.0f);
	std::uniform_real_distribution<float> m_dist_tree_angle = std::uniform_real_distribution<float>(-15.0f, 15.0f);
	std::uniform_int_distribution<int>    m_dist_tree_texture_index = std::uniform_int_distribution<int>(0, 2);

	float                              m_footstep_timer_cooldown = 0.85f;
	float                              m_footstep_timer = m_footstep_timer_cooldown;
	std::uniform_int_distribution<int> m_random_footstep_sound = std::uniform_int_distribution<int>(1, 5);

	// How long should the transition last?
	float transition_length = 3.0f;
	// A running counter of how much time if left in the transition.
	float transition_timer = transition_length;
	float m_transparency = 1.f;

	bool m_show_bounding_boxes = false;
};
