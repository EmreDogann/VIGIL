#pragma once
#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"
#include "AnimationController.h"

class PlayerController {
public:
	PlayerController(engine::ref<engine::skinned_mesh>& skinnedMesh);
	~PlayerController();

	void on_update(const engine::timestep& time_step, engine::game_object& m_player);
	void on_event(engine::event& event);

	void move(float moveDirection, const engine::timestep& time_step, engine::game_object& m_player);
	void turn(float turnAmount, const engine::timestep& time_step, engine::game_object& m_player);

	void start_taking_damage();

	void play_pickup(int anim_index) {
		m_animController->playAnimation(anim_index);
		m_pickup = true;

		m_moving_forward = false;
		m_aiming_up = false;
		m_aiming_down = false;
		m_shooting = false;
		m_animController->loopAnimation(0); // Loop default idle animation.
	}

	void unpause_animation() {
		m_animController->pauseAnimation(false);
	}

	bool is_pickup_done() {
		return !m_pickup;
	}

	bool is_ready_to_shoot() {
		return !m_shooting && m_aiming_up;
	}

	void switch_meshes(engine::ref<engine::skinned_mesh>& new_skinnedMesh) {
		m_animController->set_skinned_mesh(new_skinnedMesh);
		m_animController->loopAnimation(0);
		m_animController->playAnimation(0);
	}

private:
	float m_move_speed = 0.6f;
	float m_turn_speed = 150.f;

	bool m_moving_forward = false; // Is player moving forward.

	bool m_aiming_up = false;   // Is player aiming.
	bool m_aiming_down = false; // Is player currently aiming down.
	bool m_shooting = false;    // Is playing shooting.

	bool  m_taking_damage = false; // Is player taking damage.
	float m_taking_damage_timer = 0.0f;

	bool  m_pickup = false; // Is player picking up an item.
	float m_extra_pickup_delay = 0.3f;
	float m_pickup_delay_timer = m_extra_pickup_delay;

	AnimState m_saved_state;

	engine::ref<AnimationController> m_animController{};
};
