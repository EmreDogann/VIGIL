#include "pch.h"
#include "Enemy.h"

Enemy::Enemy(const engine::game_object_properties& props, glm::vec3 position, glm::vec3 forward) :
	game_object(props) {

	this->set_forward(forward);
	this->set_position(position);
	this->set_rotation_amount(atan2(forward.x, forward.z));

	m_animController = AnimationController::create(this->animated_mesh());
	m_bounding_boxes.push_back(std::make_shared<engine::bounding_box>(bounding_shape().x * scale().x, bounding_shape().y * scale().y, bounding_shape().z * scale().z,
	                                                                  this->position()));

	m_audio_manager = engine::application::audio();

	// Assign a random cooldown timer for the groan sound effect.
	m_groan_cooldown = m_random_cooldown(mt);
	m_groan_timer = m_groan_cooldown;
}

Enemy::~Enemy() {}

engine::ref<Enemy> Enemy::create(glm::vec3 position, glm::vec3 forward) {
	// Create enemy game object and load mesh & animations.
	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/enemy/zombie.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/enemy/idle.fbx");   // 0
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/enemy/walk.fbx");   // 1
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/enemy/attack.fbx"); // 2
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/enemy/hit.fbx");    // 3
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/enemy/dead.fbx");   // 4
	m_skinned_mesh->switch_root_movement(false);
	m_skinned_mesh->set_default_animation(0);

	engine::game_object_properties enemy_props;
	enemy_props.animated_mesh = m_skinned_mesh;
	enemy_props.scale = glm::vec3(0.51f); // Make enemy slightly taller than player to make them more menacing.
	enemy_props.textures = {engine::texture_2d::create("assets/models/animated/enemy/zombie.png", false)};
	enemy_props.type = 0;
	enemy_props.bounding_shape = glm::vec3(0.6f, 1.8f, 0.6f);
	return std::make_shared<Enemy>(enemy_props, position, forward);
}

void Enemy::on_update(const engine::timestep& time_step, Player& player, engine::perspective_camera& camera) {
	m_animController->on_update(time_step);
	float distance_to_player = glm::distance(this->position(), player.position());

	m_attack_timer -= time_step;
	if (m_attacking) m_deal_damage_delay_timer -= time_step;
	// Check which state is the Enemy in, then execute the matching behaviour
	if (m_state == state::idle) {
		idle(time_step, camera);

		// If the player gets close enough to the enemy, switch to the chase state and start chasing the player.
		if (distance_to_player < m_detection_radius) {
			m_state = state::chase;
			m_animController->loopAnimation(1); // Play & Loop walking animation.
			m_animController->playAnimation(1);
		}
	} else if (m_state == state::chase) {
		// If the player is close enough to the enemy while they are chasing the player, switch to the attack state.
		if (distance_to_player < m_attack_radius) {
			if (m_attack_timer <= 0.0f) {
				m_state = state::attack;
			}
		} else {
			chase_player(time_step);
		}
	} else if (m_state == state::attack) {
		// Return to chase state if the player moves out of the attack radius.
		// This also has the side bonus of making the enemy miss their attack if the player can move out of the way in time.
		if (distance_to_player > m_attack_radius) {
			m_state = state::chase;

			m_deal_damage_delay_timer = m_deal_damage_delay;
			m_attack_timer = m_attack_cooldown;
			m_attacking = false;
		} else {
			if (!m_attacking) {
				if (m_attack_timer <= 0.0f) {
					attack_player(time_step, player, camera);
					m_attack_timer = m_attack_cooldown; // Prevent enemy from attacking multiple times in quick succession.
				}
			} else {
				if (m_deal_damage_delay_timer <= 0.0f) {
					player.apply_damage(25.0f, camera);
					m_attacking = false;
					m_deal_damage_delay_timer = m_deal_damage_delay;
				}
			}
		}
	} else if (m_state == state::took_hit) {
		took_hit(time_step);
	} else if (m_state == state::dead) {
		dead(time_step);
	}
}

void Enemy::on_render(const engine::ref<engine::shader>& shader, engine::ref<Enemy>& enemy) {
	glm::mat4 enemy_transform(1.0f);
	enemy_transform = glm::translate(enemy_transform, this->position());
	enemy_transform = glm::rotate(enemy_transform, this->rotation_amount(), this->rotation_axis());
	enemy_transform = glm::scale(enemy_transform, this->scale());

	engine::renderer::submit(shader, enemy_transform, enemy);
}

// Update the path to the player the enemy needs to take.
void Enemy::update_path(const std::vector<glm::vec3> path_to_player) {
	m_current_path = path_to_player;
	m_current_path_node = 1;
}

// Play random groan sound while in idle state.
void Enemy::idle(const engine::timestep& time_step, engine::perspective_camera& camera) {
	m_groan_timer -= time_step;
	if (m_groan_timer <= 0.0f) {
		m_audio_manager->play_spatialised_sound("zombie_groan" + std::to_string(m_random_groan(mt)), camera.position(), position());
		m_groan_timer = m_groan_cooldown;
	}
}

// Linearly interpolate between two different floating point numbers.
float lerp(float a, float b, float f) {
	return (a * (1.0f - f)) + (b * f);
}

// Move forwards in the direction of the next node in the path.
void Enemy::chase_player(const engine::timestep& time_step) {
	// Get the current node position.
	glm::vec3 current_node_pos = {m_current_path[m_current_path_node].x, this->position().y, m_current_path[m_current_path_node].z};
	m_current_node_progress = glm::distance(this->position(), current_node_pos);

	// If the enemy is 'close enough' to the current node, move onto the next node. This is done to slightly smooth out the movement and make it a little more realistic (relatively).
	if (m_current_node_progress <= 0.1f) {
		m_current_path_node++;
	}

	// Linearly interpolate forward and by extension the rotation amount.
	glm::vec3 new_forward = glm::normalize(current_node_pos - position());
	set_forward({lerp(forward().x, new_forward.x, time_step), new_forward.y, lerp(forward().z, new_forward.z, time_step)});

	set_position(position() + forward() * m_speed * (float)time_step); // Move forward a little bit.
	m_bounding_boxes[0]->on_update(this->position());                  // Update bounding box to the new position.

	set_rotation_amount(atan2(forward().x, forward().z));
}

void Enemy::attack_player(const engine::timestep& time_step, Player& player, engine::perspective_camera& camera) {
	m_animController->playAnimation(2); // Play attack animation.
	m_audio_manager->play_spatialised_sound("zombie_attack", camera.position(), position());
	m_attacking = true;
}

void Enemy::took_hit(const engine::timestep& time_step) {
	if (m_animController->current_animation_remaining_time() - time_step <= 0.0f) {
		m_state = state::chase;
		m_animController->loopAnimation(1); // Loop walking animation.
		m_animController->playAnimation(1);
	}
}

// Don't do anything after the enemy has died.
void Enemy::dead(const engine::timestep& time_step) {
	if (m_animController->current_animation_remaining_time() - time_step <= 0.0f) {
		m_animController->pauseAnimation(true);
	}
}

void Enemy::apply_damage(const float damage, engine::perspective_camera& camera) {
	m_health -= damage;
	if (m_health <= 0) {
		m_state = state::dead;
		m_audio_manager->play_spatialised_sound("zombie_dying", camera.position(), position());
		m_animController->playAnimation(4); // Play hit animation.
	} else {
		m_state = state::took_hit;
		m_animController->playAnimation(3); // Play hit animation.
	}
}
