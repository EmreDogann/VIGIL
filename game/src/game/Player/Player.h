#pragma once
#include "PlayerController.h"

#include "game/Pickup/Pistol.h"
#include "game/Pickup/FirstAidSpray.h"
#include "game/Pickup/Ammo.h"

class Player : public engine::game_object {
public:
	Player(const engine::game_object_properties& props, engine::ref<engine::skinned_mesh>& skinnedMesh);
	~Player();

	static engine::ref<Player> create();

	void on_update(const engine::timestep& time_step, engine::ref<engine::bullet_manager> physics_manager);
	void on_render(const engine::ref<engine::shader>& shader, engine::ref<Player>& m_player);
	void on_event(engine::event& event, engine::ref<engine::bullet_manager> physics_manager, engine::perspective_camera& camera);

	void pickup_item(int anim_index) {
		m_playerController->play_pickup(anim_index);
	}

	void unpause_animations() {
		m_playerController->unpause_animation();
	}

	bool is_pickup_anim_finished() {
		return m_playerController->is_pickup_done();
	}

	bool is_ready_to_shoot() {
		return m_playerController->is_ready_to_shoot();
	}

	void addToInventory(const int item_type) {
		if (item_type == 0) {
			// Gun
			engine::ref<Pistol> p_pistol = std::make_shared<Pistol>();
			m_inventory.push_back(p_pistol);
			p_pistol->set_inventory_position(m_inventory.size() - 1);
		} else if (item_type == 1) {
			// First Aid Spray
			engine::ref<FirstAidSpray> p_first_aid_spray = std::make_shared<FirstAidSpray>();
			m_inventory.push_back(p_first_aid_spray);
			p_first_aid_spray->set_inventory_position(m_inventory.size() - 1);
		} else if (item_type == 2) {
			// Ammo
			engine::ref<Ammo> p_ammo = std::make_shared<Ammo>();
			m_inventory.push_back(p_ammo);
			p_ammo->set_inventory_position(m_inventory.size() - 1);
		}
	}

	void removeFromInventory(const engine::ref<Pickup>& item) {
		m_inventory.erase(std::remove(m_inventory.begin(), m_inventory.end(), item), m_inventory.end());
	}

	std::vector<engine::ref<Pickup>> get_inventory() {
		return m_inventory;
	}

	bool is_inventory_full() {
		return m_inventory.size() == m_inventory_capacity;
	}

	void equip_weapon(engine::ref<Pickup> item) {
		if (item->get_object()->type() == 0) {
			m_equipped_weapon = item;
			this->set_animated_mesh(m_skinned_mesh_gun);
			m_playerController->switch_meshes(m_skinned_mesh_gun);
		}
	}

	void unequip_weapon() {
		m_equipped_weapon = nullptr;
		this->set_animated_mesh(m_skinned_mesh);
		m_playerController->switch_meshes(m_skinned_mesh);
	}

	bool is_weapon_equipped() {
		return m_equipped_weapon != nullptr;
	}

	std::vector<engine::ref<engine::game_object>> get_bullet_objects() {
		if (is_weapon_equipped()) return m_equipped_weapon->get_associated_objects();
		return std::vector<engine::ref<engine::game_object>>();
	}

	float get_health() {
		return m_health;
	}

	void set_health(float health) {
		m_health = health;
		m_health = std::clamp(m_health, 0.0f, 100.f);
	}

	void apply_damage(const float damage, engine::perspective_camera& camera);
	bool is_dead = false;

private:
	engine::ref<PlayerController>      m_playerController;
	engine::ref<engine::audio_manager> m_audio_manager;

	float m_health = 30.0f;

	engine::ref<Pickup>               m_equipped_weapon = nullptr;
	engine::ref<engine::skinned_mesh> m_skinned_mesh;
	engine::ref<engine::skinned_mesh> m_skinned_mesh_gun;

	int                              m_inventory_capacity = 8;
	std::vector<engine::ref<Pickup>> m_inventory;
	engine::ref<engine::material>    m_material;
};
