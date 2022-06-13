#include "Death_Layer.h"

#include <iomanip>
#include <gl/GL.h>

#include "Level2_Layer.h"
#include "MainMenu_Layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"

// Taken from: https://stackoverflow.com/a/45885734/10439539. Turns seconds in hh::mm::ss.
std::string get_formatted_time(double seconds) {
	double s(fabs(seconds));
	int    h(s / 3600);
	int    min(s / 60 - h * 60);
	double sec(s - (h * 60 + min) * 60);

	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << fabs(seconds) / seconds * h << ":" << std::setw(2) << min << ":";
	if (sec / 10 < 1)
		oss << "0";
	oss << sec;

	return oss.str();
}

// Taken from: https://www.geeksforgeeks.org/rounding-floating-point-number-two-decimal-places-c-c/
float round_float(float var) {
	float value = (int)(var * 100 + .5);
	return (float)value / 100;
}

Death_Layer::Death_Layer() :
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f) {
	// Hide the mouse and lock it inside the window
	engine::application::window().show_mouse_cursor();

	// Initialise the shaders.
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Set default mesh shader parameters.
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);


	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
	                                                                       glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
	                                                                                  (float)engine::application::window().height()));

	// Create background image.
	engine::game_object_properties quad_props;
	quad_props.position = {0.f, 0.f, -0.1f};
	quad_props.meshes = {engine::quad::create({(float)engine::application::window().width() / (float)engine::application::window().height(), 1}, -1)->mesh()};
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Background.png", false)};
	quad_props.type = 1;
	background_image = engine::game_object::create(quad_props);

	// Create transition image used for crossfading.
	quad_props.position = {0.f, 0.f, 2.0f};
	quad_props.textures = {engine::texture_2d::create("assets/textures/transition_image.png", false)};
	transition_image = engine::game_object::create(quad_props);

	// Create dead text image.
	quad_props.position = {0.0f, 0.15f, 0.1f};
	quad_props.scale = glm::vec3(4.0f, 1.0f, 1.0f) / 8.0f;
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Death-Screen-Text.png", false)};
	dead_text_image = engine::game_object::create(quad_props);

	m_text_manager = engine::text_manager::create("BebasNeue-Bold.ttf");

	m_audio_manager = engine::application::audio();

	m_button_manager = ButtonManager::create(m_2d_camera.view_projection_matrix());

	// Create text with playtime.
	m_button_manager->create_text_button("Playtime:    " + get_formatted_time(round_float(engine::application::play_time())), 1.0f, {0.0f, -0.1f}, {1.f, 1.f, 1.f, 0.9f}, {1.f, 1.f, 1.f, 0.9f}, m_text_manager, []()
	                                     {}
	);

	// Create retry button which will reload level 2 when clicked.
	m_button_manager->create_text_button("Retry", 1.0f, {0.0f, -0.25f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, [this]()
	                                     {
		                                     m_button_manager->set_active(false);
		                                     on_transition_finish = [this]()
		                                     {
			                                     m_audio_manager->stop_all();
			                                     engine::application::play_time() = 0.0f;
			                                     engine::application::instance().pop_layer_buffer(this);
			                                     engine::application::instance().push_layer_buffer(new Level2_layer());
		                                     };
		                                     detach_transition_in_progress = true;
	                                     }
	);

	// Create exit button which will load the main menu when clicked.
	m_button_manager->create_text_button("Exit", 1.0f, {0.0f, -0.355f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, [this]()
	                                     {
		                                     m_button_manager->set_active(false);
		                                     on_transition_finish = [this]()
		                                     {
			                                     m_audio_manager->stop_all();
			                                     engine::application::play_time() = 0.0f;
			                                     engine::application::instance().pop_layer_buffer(this);
			                                     engine::application::instance().push_layer_buffer(new MainMenu_Layer());
		                                     };
		                                     detach_transition_in_progress = true;
	                                     }
	);

	// Start Fade-In transition
	attach_transition_in_progress = true;
	is_layer_active = true;
}

Death_Layer::~Death_Layer() {}

void Death_Layer::attach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = transition_timer / transition_length;

	if (transition_timer <= 0) {
		attach_transition_in_progress = false;

		on_transition_finish();
		on_transition_finish = []()
			{};

		transition_timer = transition_length;
		m_transparency = 0.f;
		m_button_manager->set_active(true);
	}
}

void Death_Layer::detach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = 1.f - transition_timer / transition_length;

	if (transition_timer <= 0) {
		detach_transition_in_progress = false;
		is_layer_active = false;

		on_transition_finish();
		on_transition_finish = []()
			{};

		transition_timer = transition_length;
	}
}

void Death_Layer::on_update(const engine::timestep& time_step) {
	if (attach_transition_in_progress) {
		attach_scene_transition(time_step);
	} else if (detach_transition_in_progress) {
		detach_scene_transition(time_step);
	}

	m_2d_camera.on_update(time_step);
}

void Death_Layer::on_render() {
	engine::render_command::clear_color({0.f, 0.f, 0.f, 1.0f});
	engine::render_command::clear();

	// Set up shader.
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	glEnable(GL_ALPHA_TEST);
	// Render buttons
	m_button_manager->on_render_text(text_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind(); // Rebind mesh shader other it will try to use text_shader to render everything else.

	// Render background image.
	engine::renderer::submit(mesh_shader, background_image);

	// Render 'YOU ARE DEAD' text.
	engine::renderer::submit(mesh_shader, dead_text_image);

	// Render transition image.
	if (attach_transition_in_progress || detach_transition_in_progress) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void Death_Layer::on_event(engine::event& event) {
	if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		// Check if button has been pressed.
		if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
			m_button_manager->check_confirm();
		}
	} else if (event.event_type() == engine::event_type_e::mouse_moved) {
		auto& e = dynamic_cast<engine::mouse_moved_event&>(event);
		// Check if button has been hovered over.
		m_button_manager->check_hover_text(e.x(), e.y());
	}
}
