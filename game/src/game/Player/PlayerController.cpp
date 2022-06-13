#include "PlayerController.h"
#include "engine/core/input.h"
#include <engine/events/key_event.h>

#include "engine/mouse_buttons_codes.h"
#include "engine/events/mouse_event.h"

PlayerController::PlayerController(engine::ref<engine::skinned_mesh>& skinnedMesh) :
	m_saved_state(AnimState(0, 0.0f, false)) {
	m_animController = AnimationController::create(skinnedMesh);
};

PlayerController::~PlayerController() {};

void PlayerController::on_update(const engine::timestep& time_step, engine::game_object& m_player) {
	// If player is taking damage, override all other animations.
	if (m_taking_damage) {
		m_taking_damage_timer -= time_step;
		if (m_taking_damage_timer <= 0.0f) {
			m_taking_damage = false;
			m_animController->set_state(m_saved_state);
		}
	} else {
		if (m_pickup) {
			// Add a little extra delay before considering the animation finished in order to make it a little less jarring.
			if (m_animController->is_anim_paused()) {
				m_pickup_delay_timer -= time_step;
				if (m_pickup_delay_timer <= 0.0f) {
					m_pickup_delay_timer = m_extra_pickup_delay;
					m_pickup = false;
				}
			} else if (m_animController->current_animation_remaining_time() - time_step <= 0.0f) {
				m_animController->pauseAnimation(true);
			}
		} else {
			if (m_aiming_up) {
				if (m_animController->current_animation_remaining_time() - time_step <= 0.0f) {
					if (!m_shooting) m_animController->pauseAnimation(true);
					else {
						m_animController->set_state(m_saved_state);
						m_shooting = false;
					}
				}
			} else if (m_aiming_down) {
				if (m_animController->current_animation_remaining_time() - time_step <= 0.0f) {
					m_aiming_down = false;
				}
			}

			if (engine::input::key_pressed(engine::key_codes::KEY_W) && !m_aiming_up && !m_aiming_down) {
				move(1, time_step, m_player);

				if (!m_moving_forward && !m_animController->is_anim_in_proress()) {
					m_moving_forward = true;
					m_animController->queueAnimation(1);
					m_animController->loopAnimation(2);
				}
			} else if (engine::input::key_pressed(engine::key_codes::KEY_A)) {
				turn(1, time_step, m_player);
			} else if (engine::input::key_pressed(engine::key_codes::KEY_D)) {
				turn(-1, time_step, m_player);
			}
		}
	}
	m_animController->on_update(time_step);
};

void PlayerController::move(float moveDirection, const engine::timestep& time_step, engine::game_object& m_player) {
	m_player.set_position(m_player.position() + (moveDirection * time_step * m_move_speed * m_player.forward()));
}

void PlayerController::turn(float turnAmount, const engine::timestep& time_step, engine::game_object& m_player) {
	// Give new forward to player model using the new angle in radians.
	m_player.set_forward(glm::rotate(m_player.forward(), glm::radians((turnAmount * time_step * m_turn_speed)), m_player.rotation_axis()));
	// Use this new forward to calculate the amount to rotate by.
	m_player.set_rotation_amount(atan2(m_player.forward().x, m_player.forward().z));
}

void PlayerController::start_taking_damage() {
	m_taking_damage = true;
	m_saved_state = m_animController->save_state();

	if (m_aiming_up) {
		m_animController->pauseAnimation(false);
		m_animController->playAnimation(6);
	} else {
		m_animController->playAnimation(7);
	}
	m_taking_damage_timer = m_animController->current_animation_remaining_time();
}

void PlayerController::on_event(engine::event& event) {
	if (event.event_type() == engine::event_type_e::key_released) {
		auto& e = dynamic_cast<engine::key_released_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_W && !m_aiming_up && !m_aiming_down && !m_pickup) {
			m_moving_forward = false;
			m_animController->playAnimation(3);
			m_animController->loopAnimation(0);
		}
	}

	if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		if (!m_pickup) {
			if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_RIGHT) {
				m_animController->playAnimation(4);
				m_aiming_up = true;

				if (m_moving_forward) {
					m_moving_forward = false;
					m_animController->loopAnimation(0);
				}
			} else if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_LEFT && m_aiming_up && m_animController->is_anim_paused() && !m_shooting) {
				m_saved_state = m_animController->save_state();
				m_animController->pauseAnimation(false);
				m_animController->playAnimation(10);
				m_shooting = true;
			}
		}
	}

	if (event.event_type() == engine::event_type_e::mouse_button_released) {
		auto& e = dynamic_cast<engine::mouse_button_released_event&>(event);
		if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_RIGHT && !m_pickup) {
			if (m_taking_damage) {
				m_saved_state = AnimState(5, 0.0f, true, m_animController->calculate_animation_length(5), 0.0f);
			} else {
				m_animController->pauseAnimation(false);
				m_animController->playAnimation(5);
			}
			m_aiming_up = false;
			m_aiming_down = true;
		}
	}
};
