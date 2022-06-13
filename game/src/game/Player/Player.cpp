#include "Player.h"

#include "engine/entities/bounding_box.h"
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"

Player::Player(const engine::game_object_properties& props, engine::ref<engine::skinned_mesh>& skinnedMesh) :
	game_object(props),
	m_skinned_mesh(skinnedMesh),
	m_material(engine::material::create(
		0.0f, glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.f),
		1.0f)) {
	m_playerController = std::make_shared<PlayerController>(skinnedMesh);
	m_bounding_boxes.push_back(std::make_shared<engine::bounding_box>(bounding_shape().x * scale().x, bounding_shape().y * scale().y, bounding_shape().z * scale().z,
	                                                                  position()));

	m_audio_manager = engine::application::audio();

	m_skinned_mesh_gun = engine::skinned_mesh::create("assets/models/animated/player/model-gun.fbx");
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/idle.fbx");         // 0
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/walkingStart.fbx"); // 1
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/walking.fbx");      // 2
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/walkingEnd.fbx");   // 3
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/aim.fbx");          // 4
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/aimDown.fbx");      // 5
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/aimHit.fbx");       // 6
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/hit.fbx");          // 7
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/pickup.fbx");       // 8
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/pickup2.fbx");      // 9
	m_skinned_mesh_gun->LoadAnimationFile("assets/models/animated/player/shoot.fbx");        // 10
	m_skinned_mesh_gun->set_default_animation(0);
}

Player::~Player() {}

engine::ref<Player> Player::create() {
	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/player/model.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/idle.fbx");         // 0
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/walkingStart.fbx"); // 1
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/walking.fbx");      // 2
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/walkingEnd.fbx");   // 3
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/aim.fbx");          // 4
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/aimDown.fbx");      // 5
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/aimHit.fbx");       // 6
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/hit.fbx");          // 7
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/pickup.fbx");       // 8
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/pickup2.fbx");      // 9
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/player/shoot.fbx");        // 10
	m_skinned_mesh->set_default_animation(0);

	engine::game_object_properties player_props;
	player_props.animated_mesh = m_skinned_mesh;
	player_props.scale = glm::vec3(.45f);
	player_props.textures = {engine::texture_2d::create("assets/models/animated/player/man_t256.png", false)};
	player_props.type = 0;
	player_props.bounding_shape = glm::vec3(0.4f, 1.8f, 0.4f);
	return std::make_shared<Player>(player_props, m_skinned_mesh);
}

void Player::on_update(const engine::timestep& time_step, engine::ref<engine::bullet_manager> physics_manager) {
	m_playerController->on_update(time_step, *this);
	m_bounding_boxes[0]->on_update(this->position());

	if (is_weapon_equipped()) {
		physics_manager->dynamics_world_update(m_equipped_weapon->get_associated_objects(), static_cast<double>(time_step));
		for (auto& bullet : m_equipped_weapon->get_associated_objects()) {
			bullet->bounding_box()[0]->on_update(bullet->position());
		}
	}
}

void Player::on_render(const engine::ref<engine::shader>& shader, engine::ref<Player>& m_player) {

	m_material->submit(shader);

	glm::mat4 player_transform(1.0f);
	player_transform = glm::translate(player_transform, this->position());
	player_transform = glm::rotate(player_transform, this->rotation_amount(), this->rotation_axis());
	player_transform = glm::scale(player_transform, this->scale());

	engine::renderer::submit(shader, player_transform, m_player);

	if (is_weapon_equipped()) {
		for (auto& bullet : m_equipped_weapon->get_associated_objects()) {
			glm::mat4 bullet_transform(1.0f);
			bullet_transform = glm::translate(bullet_transform, bullet->position());
			bullet_transform = glm::rotate(bullet_transform, bullet->rotation_amount(), bullet->rotation_axis());
			bullet_transform = glm::scale(bullet_transform, bullet->scale());

			engine::renderer::submit(shader, bullet_transform, bullet);
		}
	}
}

void Player::on_event(engine::event& event, engine::ref<engine::bullet_manager> physics_manager, engine::perspective_camera& camera) {
	// Only enable shooting controls if the player has a weapon equipped.
	if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		if (is_weapon_equipped()) {
			if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_LEFT && m_equipped_weapon->get_quantity() == 0) {
				m_audio_manager->play_spatialised_sound("gunshot_empty", camera.position(), position());
			} else if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_LEFT && m_equipped_weapon->get_quantity() > 0 && is_ready_to_shoot()) {
				m_equipped_weapon->useItem(); // Call the item's useItem() function. For a weapon, this will deduct its magazine size.
				auto bullet = m_equipped_weapon->get_associated_objects()[m_equipped_weapon->get_quantity()];

				// Set bullet to spawn in front of the player, with the correct rotation and update the bullet's bounding box.
				bullet->set_position(glm::vec3(position().x, position().y + scale().y * 1.5f, position().z) + scale() * forward());
				bullet->set_forward(forward());
				bullet->set_rotation_axis(glm::vec3(0.0f, 1.0f, 0.0f));
				bullet->set_rotation_amount(rotation_amount());
				bullet->set_velocity(10.0f * forward());
				bullet->bounding_box()[0]->on_update(bullet->position());

				physics_manager->add_physical_object(bullet, physics_manager->get_dynamics_world());
				m_audio_manager->play_spatialised_sound("gunshot", camera.position(), position()); // Play gunshot sound at the position of the player.

				m_playerController->on_event(event);
			} else if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_RIGHT) {
				m_playerController->on_event(event);
			}
		}
	} else if (event.event_type() == engine::event_type_e::mouse_button_released) {
		auto& e = dynamic_cast<engine::mouse_button_released_event&>(event);
		if (is_weapon_equipped()) {
			m_playerController->on_event(event);
		}
	} else {
		m_playerController->on_event(event);
	}
}

void Player::apply_damage(const float damage, engine::perspective_camera& camera) {
	m_health -= damage;
	if (m_health <= 0.0f) {
		is_dead = true;
	}
	m_playerController->start_taking_damage();
	m_audio_manager->play_spatialised_sound("player_hit", camera.position(), position());
}
