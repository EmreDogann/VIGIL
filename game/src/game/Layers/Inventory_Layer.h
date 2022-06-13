#pragma once
#include <engine.h>

#include "engine/utils/ButtonManager.h"
#include "game/Player/Player.h"

struct InventorySlot {
	InventorySlot(engine::ref<engine::game_object> image, engine::ref<Pickup> item) :
		slot_image(image), item(item) {};
	~InventorySlot() {};

	engine::ref<engine::game_object> slot_image;
	engine::ref<Pickup>              item;
};

class Inventory_Layer : public engine::layer {
public:
	Inventory_Layer(engine::ref<Player>& player_ref);
	~Inventory_Layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void detach_scene_transition(const engine::timestep& time_step) override;
	void attach_scene_transition(const engine::timestep& time_step) override;
	void set_default_shader_parameters() override;

	engine::ref<ButtonManager> get_button_manager() {
		return m_button_manager;
	}

	// Discard item using an item reference.
	void removeFromInventory(const engine::ref<Pickup>& item) {
		m_player->removeFromInventory(item);
		inventory_slots[m_current_active_slot].item = nullptr;
	}

	// Discard item using an inventory slot index.
	void removeFromInventory(int item_index) {
		m_player->removeFromInventory(inventory_slots[item_index].item);
		inventory_slots[item_index].item = nullptr;
	}

	// Set weapon as equipped.
	void equip_weapon(const engine::ref<Pickup>& item) {
		item->set_equipped(true);
		m_player->equip_weapon(item);

		m_audio_manager->play("ui_reveal2");
	}

	// Set weapon as unequipped.
	void unequip_weapon(const engine::ref<Pickup>& item) {
		item->set_equipped(false);
		m_player->unequip_weapon();

		m_audio_manager->play("ui_reveal2");
	}

	void examine_item();

	void heal_player(const engine::ref<Pickup>& item) {
		m_player->set_health(m_player->get_health() + item->get_quantity());
		m_audio_manager->play("ui_reveal2");
	}

	void start_combine_items();

private:
	void combine_items();

	engine::ref<engine::game_object>              background_image{};
	std::vector<InventorySlot>                    inventory_slots{};
	engine::ref<engine::game_object>              inventory_slot_selected_image{};
	engine::ref<engine::game_object>              inventory_slot_combine_first_image{};
	engine::ref<engine::game_object>              inventory_slot_combine_second_image{};
	std::vector<engine::ref<engine::game_object>> health_bar_images{};
	engine::ref<engine::game_object>              transition_image{};

	engine::ref<engine::audio_manager> m_audio_manager{};
	engine::ref<engine::text_manager>  m_text_manager{};
	engine::ref<engine::text_manager>  m_text_manager_alt{};
	engine::ref<ButtonManager>         m_button_manager{};

	engine::DirectionalLight m_directionalLight;

	engine::orthographic_camera   m_2d_camera;
	engine::ref<engine::material> m_material;

	engine::ref<Player>& m_player;

	// How long should the transition last?
	float transition_length = 0.15f;
	// A running counter of how much time if left in the transition.
	float                   transition_timer = transition_length;
	float                   m_transparency = 1.f;
	std::function<layer*()> m_next_layer;

	// Flags to see if the user is clicking and dragging the mouse.
	bool m_clicked = false;
	bool m_drag = false;

	// Store the current world to be used for rendering.
	glm::vec3 m_current_world_mouse_pos{};

	int m_current_active_slot = -1;
	int m_next_active_slot = -1;

	bool                m_combining = false;
	std::pair<int, int> m_combine_items{};
};
