#pragma once
#include <engine.h>
#include <random>

#include "game/Player/Player.h"

class Enemy : public engine::game_object {
	enum class state {
		idle,
		chase,
		attack,
		took_hit,
		dead
	};

public:
	Enemy(const engine::game_object_properties& props, glm::vec3 position, glm::vec3 forward);
	~Enemy();

	static engine::ref<Enemy> create(glm::vec3 position, glm::vec3 forward);
	// update the enemy
	void on_update(const engine::timestep& time_step, Player& player, engine::perspective_camera& camera);
	void on_render(const engine::ref<engine::shader>& shader, engine::ref<Enemy>& enemy);
	void update_path(const std::vector<glm::vec3> path_to_player);
	// methods controlling the enemy’s behaviour in a certain state
	void idle(const engine::timestep& time_step, engine::perspective_camera& camera);

	void chase_player(const engine::timestep& time_step);
	void attack_player(const engine::timestep& time_step, Player& player, engine::perspective_camera& camera);
	void took_hit(const engine::timestep& time_step);
	void dead(const engine::timestep& time_step);

	void apply_damage(const float damage, engine::perspective_camera& camera);

	bool is_dead() {
		return m_state == state::dead;
	}

	bool is_taking_damage() {
		return m_state == state::took_hit;
	}

private:
	// Enemy's speed
	float m_speed{0.25f};

	// Amount of time to wait before attacking again.
	float m_attack_cooldown = 2.0f;
	float m_attack_timer = 0.0f;
	bool  m_attacking = false;

	// Amount of time to wait after entering the attack state before dealing damage to the player. This is used to sync up the enemy's attack and the player's hit animations.
	float m_deal_damage_delay = 0.8f;
	float m_deal_damage_delay_timer = m_deal_damage_delay;

	// Threshold distances
	float m_detection_radius{2.0f};
	float m_attack_radius{0.4f};

	float m_health = 100.0f;

	// Time to wait before playing a random groan sound effect.
	float m_groan_cooldown;
	float m_groan_timer;

	std::random_device rd;
	std::mt19937       mt = std::mt19937(rd());

	// Random distributions for groan sound effect.
	std::uniform_real_distribution<float> m_random_cooldown = std::uniform_real_distribution<float>(5, 15);
	std::uniform_int_distribution<int>    m_random_groan = std::uniform_int_distribution<int>(1, 2);

	// Information about the enemy's path-finding.
	std::vector<glm::vec3> m_current_path{};
	int                    m_current_path_node = 1;
	float                  m_current_node_progress = 0.0f;

	engine::ref<engine::audio_manager> m_audio_manager{};
	engine::ref<AnimationController>   m_animController{};

	// Current state of the enemy's state machine
	state m_state = state::idle;
};
