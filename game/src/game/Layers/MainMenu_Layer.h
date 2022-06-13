#pragma once
#include <engine.h>
#include "engine/utils/ButtonManager.h"

class MainMenu_Layer : public engine::layer {
public:
	MainMenu_Layer();
	~MainMenu_Layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void detach_scene_transition(const engine::timestep& time_step) override;
	void attach_scene_transition(const engine::timestep& time_step) override;

private:
	engine::ref<engine::game_object> menu_image{};
	engine::ref<engine::game_object> transition_image{};

	std::vector<engine::ref<engine::game_object>> m_game_objects{};

	engine::ref<engine::audio_manager> m_audio_manager;
	engine::ref<engine::text_manager>  m_text_manager{};
	engine::ref<ButtonManager>         m_button_manager{};

	engine::orthographic_camera m_2d_camera;

	// How long should the transition last?
	float transition_length = 4.0f;
	// A running counter of how much time if left in the transition.
	float                   transition_timer = transition_length;
	float                   m_transparency = 1.0f;
	std::function<layer*()> m_next_layer;
};
