#include "Controls_Layer.h"

#include <gl/GL.h>

#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"

Controls_Layer::Controls_Layer() :
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f) {
	// Hide the mouse and lock it inside the window
	engine::application::window().show_mouse_cursor();

	// Initialise the shaders, materials and lights
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

	// Create transition image used for cross-fading.
	quad_props.position = {0.f, 0.f, 2.0f};
	quad_props.textures = {engine::texture_2d::create("assets/textures/transition_image.png", false)};
	transition_image = engine::game_object::create(quad_props);

	// Create controls image.
	quad_props.position = {0.0f, 0.0f, 0.1f};
	quad_props.scale = glm::vec3(0.85f);
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Controls.png", false)};
	controls_image = engine::game_object::create(quad_props);

	m_text_manager = engine::text_manager::create("ArchivoNarrow-Bold.ttf");

	// Retrieve audio manager instance
	m_audio_manager = engine::application::audio();

	m_button_manager = ButtonManager::create(m_2d_camera.view_projection_matrix());

	// Create a back button which will bring the user to the previous layer when pressed.
	m_button_manager->create_text_button("BACK", 1.0f, {1.1f, -0.65f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, [this]()
	                                     {
		                                     m_button_manager->set_active(false);
		                                     detach_transition_in_progress = true;
	                                     }
	);


	// Start Fade-In transition
	attach_transition_in_progress = true;
	is_layer_active = true;
}

Controls_Layer::~Controls_Layer() {}

void Controls_Layer::attach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = transition_timer / transition_length;

	if (transition_timer <= 0) {
		attach_transition_in_progress = false;
		transition_timer = transition_length;
		m_transparency = 0.f;
		m_button_manager->set_active(true);
	}
}

// Remove layer when fade out transition is completed.
void Controls_Layer::detach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = 1.f - transition_timer / transition_length;

	if (transition_timer <= 0) {
		engine::application::instance().pop_layer_buffer(this);
		transition_timer = transition_length;
	}
}

void Controls_Layer::on_update(const engine::timestep& time_step) {
	if (attach_transition_in_progress) {
		attach_scene_transition(time_step);
	} else if (detach_transition_in_progress) {
		detach_scene_transition(time_step);
	}

	m_2d_camera.on_update(time_step);
}

void Controls_Layer::on_render() {
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

	// Render controls image.
	engine::renderer::submit(mesh_shader, controls_image);

	// Render transition image.
	if (attach_transition_in_progress || detach_transition_in_progress) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void Controls_Layer::on_event(engine::event& event) {
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
