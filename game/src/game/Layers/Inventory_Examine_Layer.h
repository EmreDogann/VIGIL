#pragma once
#include <engine.h>
#include <random>

#include "engine/utils/ButtonManager.h"
#include "game/Player/Player.h"

class Inventory_Examine_Layer : public engine::layer {
public:
	Inventory_Examine_Layer(engine::ref<engine::game_object> item_ref);
	~Inventory_Examine_Layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void detach_scene_transition(const engine::timestep& time_step) override;
	void attach_scene_transition(const engine::timestep& time_step) override;

private:
	engine::ref<engine::game_object> background_image{};
	engine::ref<engine::game_object> transition_image{};

	engine::ref<engine::audio_manager> m_audio_manager{};
	engine::ref<engine::text_manager>  m_text_manager{};
	engine::ref<ButtonManager>         m_button_manager{};

	engine::DirectionalLight m_directionalLight;

	engine::orthographic_camera m_2d_camera;

	// References item.
	engine::ref<engine::game_object> m_item;

	// How long should the transition last?
	float transition_length = 0.4f;
	// A running counter of how much time if left in the transition.
	float transition_timer = transition_length;
	float m_transparency = 1.f;

	std::random_device rd;
	std::mt19937       mt = std::mt19937(rd());

	//Random distributions for rotations.
	std::uniform_real_distribution<float> m_random_angle = std::uniform_real_distribution<float>(engine::PI / 2.0f, engine::PI);
	std::vector<glm::quat>                m_rotations;
	float                                 m_rotation_progress = 0.0f;
	int                                   m_current_quat = 1;

	float m_scale = 0.0f;
	float m_target_scale = 5.0f;
	bool  m_item_transformation_in_progress = false;
	bool  m_item_reset_transformation_in_progress = false;

	// Flags to see if the user is clicking and dragging the mouse.
	bool m_clicked = false;
	bool m_drag = false;
	bool m_hover = false;

	// The model rotation matrix to use to render the item.
	glm::mat4 model_rotation{1.0f};

	glm::vec3 m_prev_mouse_position{0.0f};    // The mouse position in the previous frame.
	glm::vec3 m_start_mouse_position{0.0f};   // The mouse position at the point the user clicks and starts dragging.
	glm::vec3 m_current_mouse_position{0.0f}; // The mouse position in the current frame.
	glm::mat4 m_drag_rotation{1.0f};          // The rotation matrix of the current dragging action.
};
