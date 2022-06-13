#pragma once
#include <engine.h>

#include "Inventory_Layer.h"
#include "engine/utils/ButtonManager.h"
#include "game/Player/Player.h"

class Inventory_Details_Layer : public engine::layer {
public:
	Inventory_Details_Layer(glm::vec3 origin, engine::orthographic_camera& camera, engine::ref<Pickup>& item_ref, Inventory_Layer* inventory_layer);
	~Inventory_Details_Layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void set_default_shader_parameters() override;

private:
	std::vector<std::pair<std::string, std::function<void()>>> m_available_options;

	std::vector<engine::ref<engine::game_object>> options_images{};
	engine::ref<engine::game_object>              options_selected_image{};

	engine::ref<engine::audio_manager> m_audio_manager{};
	engine::ref<engine::text_manager>  m_text_manager{};
	engine::ref<ButtonManager>         m_button_manager{};

	engine::DirectionalLight m_directionalLight;

	engine::orthographic_camera&  m_2d_camera;
	engine::ref<engine::material> m_material;

	glm::vec3            origin;
	Inventory_Layer*     inventory_layer; // Pointer to inventory layer instance.
	engine::ref<Pickup>& m_item;

	int m_current_active_slot = -1;
};
