#include "MainMenu_Layer.h"
#include "Level1_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"
#include <gl/GL.h>

#include "Controls_Layer.h"

MainMenu_Layer::MainMenu_Layer() :
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f) {
	// Hide the mouse and lock it inside the window.
	engine::application::window().show_mouse_cursor();

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Set default mesh shader parameters.
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);


	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
	                                                                       glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
	                                                                                  (float)engine::application::window().height()));

	std::vector<engine::ref<engine::texture_2d>> quad_textures = {engine::texture_2d::create("assets/textures/UI/main_menu.png", false)};
	engine::ref<engine::quad>                    quad_shape = engine::quad::create({(float)engine::application::window().width() / (float)engine::application::window().height(), 1}, -1);
	engine::game_object_properties               quad_props;
	quad_props.position = {0.f, 0.f, -0.1f};
	quad_props.meshes = {quad_shape->mesh()};
	quad_props.textures = quad_textures;
	quad_props.type = 1;
	menu_image = engine::game_object::create(quad_props);

	quad_props.position = {0.f, 0.f, 2.0f};
	quad_props.textures = {engine::texture_2d::create("assets/textures/transition_image.png", false)};
	transition_image = engine::game_object::create(quad_props);

	m_text_manager = engine::text_manager::create("IMFellDWPicaSC-Regular.ttf");

	// Initialise audio and play background music
	m_audio_manager = engine::application::audio();
	m_audio_manager->play("mainmenu_ambiance");
	//m_audio_manager->pause("background_ambiance");

	// Create Start, Options, and Exit Buttons.
	m_button_manager = ButtonManager::create(m_2d_camera.view_projection_matrix());
	m_button_manager->create_text_button("Start Game", 1.2f, {-1.18f, 0.15f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, [this]()
	                                     {
		                                     m_button_manager->set_active(false);
		                                     m_next_layer = [this]()
		                                     {
			                                     engine::application::instance().pop_layer_buffer(this);
			                                     return new Level1_layer();
		                                     };
		                                     transition_length = 2.0f;
		                                     transition_timer = transition_length;
		                                     detach_transition_in_progress = true;
	                                     }
	);
	m_button_manager->create_text_button("Options", 1.2f, {-1.25f, 0.0f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, [this]()
	{
		// Deactivate the button manager while the transition is taking place.
		m_button_manager->set_active(false);
		m_next_layer = [this]() // Function to call when switching to the next layer.
		{
			m_button_manager->set_active(true);
			return new Controls_Layer();
		};

		transition_length = 0.15f;
		transition_timer = transition_length;
		detach_transition_in_progress = true;
	});
	m_button_manager->create_text_button("Exit", 1.2f, {-1.31f, -0.15f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, []()
	{
		engine::application::exit();
	});

	// Start Fade-In transition
	attach_transition_in_progress = true;
	set_is_layer_active(true);
}

MainMenu_Layer::~MainMenu_Layer() {}

void MainMenu_Layer::attach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = transition_timer / transition_length;

	if (transition_timer <= 0) {
		attach_transition_in_progress = false;
		transition_timer = transition_length;
		m_transparency = 0.f;
		m_button_manager->set_active(true);
	}
}

void MainMenu_Layer::detach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = 1.f - transition_timer / transition_length;

	if (transition_timer <= 0) {
		detach_transition_in_progress = false;
		is_layer_active = false;
		engine::application::instance().push_layer_buffer(m_next_layer());
		transition_timer = transition_length;
	}
}

void MainMenu_Layer::on_update(const engine::timestep& time_step) {
	if (attach_transition_in_progress) {
		attach_scene_transition(time_step);
	} else if (detach_transition_in_progress) {
		detach_scene_transition(time_step);
	}
	// Main update code block
	m_2d_camera.on_update(time_step);
}

void MainMenu_Layer::on_render() {
	engine::render_command::clear_color({1.f, 0.f, 0.f, 1.0f});
	engine::render_command::clear();


	// Set up shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	glEnable(GL_ALPHA_TEST);

	// Render background image.
	engine::renderer::submit(mesh_shader, menu_image);

	// Render text
	m_button_manager->on_render_text(text_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind(); // Rebind mesh shader other it will try to use text_shader to render everything else.

	if (attach_transition_in_progress || detach_transition_in_progress) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void MainMenu_Layer::on_event(engine::event& event) {
	if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
			m_button_manager->check_confirm();
		}
	} else if (event.event_type() == engine::event_type_e::mouse_moved) {
		auto& e = dynamic_cast<engine::mouse_moved_event&>(event);
		m_button_manager->check_hover_text(e.x(), e.y());
	}
}
