#include "Inventory_Examine_Layer.h"

#include <gl/GL.h>

#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"

Inventory_Examine_Layer::Inventory_Examine_Layer(engine::ref<engine::game_object> item_ref) :
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_item(item_ref) {
	// Hide the mouse and lock it inside the window
	engine::application::window().show_mouse_cursor();

	// Get global audio manager.
	m_audio_manager = engine::application::audio();

	// Initialise the shaders.
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Set directional light parameters.
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.1f;
	m_directionalLight.DiffuseIntensity = 0.75f;
	m_directionalLight.Direction = glm::vec3(0.0f, -1.0f, -0.75f);

	// Set default mesh shader parameters.
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);

	m_directionalLight.submit(mesh_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
	                                                                       glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
	                                                                                  (float)engine::application::window().height()));

	// Create background image.
	engine::game_object_properties quad_props;
	quad_props.position = {0.f, 0.f, -0.5f};
	quad_props.meshes = {engine::quad::create({(float)engine::application::window().width() / (float)engine::application::window().height(), 1}, -1)->mesh()};
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Background.png", false)};
	quad_props.type = 1;
	background_image = engine::game_object::create(quad_props);

	// Create transition image used for cross-fading.
	quad_props.position = {0.f, 0.f, 2.0f};
	quad_props.textures = {engine::texture_2d::create("assets/textures/transition_image.png", false)};
	transition_image = engine::game_object::create(quad_props);

	m_text_manager = engine::text_manager::create("ArchivoNarrow-Bold.ttf");

	m_button_manager = ButtonManager::create(m_2d_camera.view_projection_matrix());

	// Set the default rotation of each pickup differently.
	glm::mat4 end_transform(1.0f);
	if (m_item->type() == 0) {
		end_transform = glm::rotate(end_transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	} else if (m_item->type() == 1) {
		end_transform = glm::rotate(end_transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	} else if (m_item->type() == 2) {
		end_transform = glm::rotate(end_transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		end_transform = glm::rotate(end_transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	m_rotations.push_back(glm::quat(end_transform)); // Rotation to end on.

	// When back button is pressed, play the randomly generated rotations in reverse while fading out. After fade out is finished, go back to inventory layer.
	m_button_manager->create_text_button("Back", 1.0f, {1.2f, -0.51f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager, [this]()
	{
		m_button_manager->set_active(false);

		// Reset transformation parameters.
		m_item_transformation_in_progress = true;
		std::reverse(m_rotations.begin(), m_rotations.end());
		m_current_quat = 1;
		m_rotation_progress = 0.0f;

		transition_length = 0.4f;
		transition_timer = transition_length;

		detach_transition_in_progress = true;
	});

	// Generate random rotations as quaternions.
	for (int i = 0; i < 9; i++) {
		m_rotations.push_back(m_rotations[m_rotations.size() - 1] * glm::quat(glm::vec3(m_random_angle(mt), m_random_angle(mt), m_random_angle(mt))));
	}

	// Reverse rotations vector to put the ending rotation at the end.
	std::reverse(m_rotations.begin(), m_rotations.end());

	// Set the default model rotation of the item game object to the ending rotation.
	model_rotation = glm::mat4_cast(m_rotations.back());

	// Start Fade-In transition
	attach_transition_in_progress = true;
	m_item_transformation_in_progress = true;
	is_layer_active = true;
}

Inventory_Examine_Layer::~Inventory_Examine_Layer() {}

void Inventory_Examine_Layer::attach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = transition_timer / transition_length;

	if (transition_timer <= 0) {
		attach_transition_in_progress = false;

		transition_timer = transition_length;
		m_transparency = 0.f;
		m_button_manager->set_active(true);
	}
}

void Inventory_Examine_Layer::detach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = 1.f - transition_timer / transition_length;

	if (transition_timer <= 0) {
		engine::application::instance().pop_layer_buffer(this);
		transition_timer = transition_length;
	}
}

void Inventory_Examine_Layer::on_update(const engine::timestep& time_step) {
	if (attach_transition_in_progress) {
		attach_scene_transition(time_step);
	} else if (detach_transition_in_progress) {
		detach_scene_transition(time_step);
	}

	// Progress through the rotation matrix. Also increase scale of the game object at the same time. Stop when reaching the end.
	if (m_item_transformation_in_progress) {
		if (m_current_quat <= m_rotations.size() - 1 && m_rotation_progress < 1.0f) m_rotation_progress += time_step * 15.0f;
		else {
			m_rotation_progress = 1.0f;
			m_item_transformation_in_progress = false;
		}

		if (m_rotation_progress >= 1.0f && m_current_quat != m_rotations.size() - 1) {
			m_rotation_progress = 0.0f;
			m_current_quat++;
		}

		if (!detach_transition_in_progress) {
			if (m_scale < m_target_scale) m_scale += time_step * 8.0f;
			else m_scale = m_target_scale;
		} else {
			// If fading out...
			if (m_scale > 0.0f) m_scale -= time_step * 8.0f;
			else m_scale = 0.0f;
		}
	}

	// Progress through the rotation matrix in reverse to reset the rotation back to it's original.
	if (m_item_reset_transformation_in_progress) {
		if (m_rotation_progress < 1.0f) m_rotation_progress += time_step * 5.0f;
		else {
			m_rotation_progress = 1.0f;
			model_rotation = glm::mat4_cast(m_rotations.back());
			m_item_reset_transformation_in_progress = false;
		}

		if (m_scale < m_target_scale) m_scale += time_step * 12.0f;
		else if (m_scale > m_target_scale) m_scale -= time_step * 12.0f;
		else m_scale = m_target_scale;
	}

	m_2d_camera.on_update(time_step);
}

void Inventory_Examine_Layer::on_render() {
	engine::render_command::clear_color({0.f, 0.f, 0.f, 1.0f});
	engine::render_command::clear();

	// Set up shader.
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	glEnable(GL_ALPHA_TEST);

	// Render background image.
	engine::renderer::submit(mesh_shader, background_image);

	glDisable(GL_CULL_FACE);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);

	glm::mat4 item_transform(1.0f);
	item_transform = glm::translate(item_transform, glm::vec3(0.0f, 0.0f, 1.0f));
	// If the item is a first aid spray, move it down as it's origin is the bottom face, not the center of the mesh.
	if (m_item->type() == 1) item_transform = glm::translate(item_transform, glm::vec3(0.0f, -0.20f, 0.0f));

	// Render game object using the calculated rotations from the rotations array.
	if (m_item_transformation_in_progress) {
		glm::quat current_quat = glm::slerp(m_rotations[m_current_quat - 1], m_rotations[m_current_quat], m_rotation_progress);

		item_transform = glm::rotate(item_transform, glm::angle(current_quat), glm::axis(current_quat));
	} else if (m_item_reset_transformation_in_progress) {
		// Render game object using the calculated rotations from the inverted rotations array. Used when fading out.
		glm::quat current_quat = glm::slerp(glm::quat(model_rotation), glm::quat(m_rotations.back()), m_rotation_progress);

		item_transform = glm::rotate(item_transform, glm::angle(current_quat), glm::axis(current_quat));
	} else {
		// Render using the model rotation matrix.
		glm::quat current_quat = glm::quat(m_drag_rotation * model_rotation);
		item_transform = glm::rotate(item_transform, glm::angle(current_quat), glm::axis(current_quat));
	}

	// Render calculated scale.
	item_transform = glm::scale(item_transform, m_item->scale() * m_scale);
	engine::renderer::submit(mesh_shader, item_transform, m_item);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	glEnable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	// Render text buttons
	m_button_manager->on_render_text(text_shader);

	// Render 'R to reset' text if the model was rotated by the player.
	if (!(attach_transition_in_progress || detach_transition_in_progress) && model_rotation != glm::mat4_cast(m_rotations.back())) {
		glm::vec4 text_dimensions;
		m_text_manager->get_text_dimensions("Press R to reset model.", 0.75f, text_dimensions);
		glm::vec2 text_pos = m_button_manager->world_to_screen(glm::vec3(0.0f, -0.85f, 0.1f));
		text_pos.x -= text_dimensions.z / 2.0f;
		m_text_manager->render_text(text_shader, "Press R to reset model.", text_pos.x, text_pos.y, 0.75f, glm::vec4(1.0f, 1.0f, 1.0f, 0.2f));
	}

	glEnable(GL_DEPTH_TEST);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind(); // Rebind mesh shader other it will try to use text_shader to render everything else.

	// Render transition image.
	if (attach_transition_in_progress || detach_transition_in_progress) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void Inventory_Examine_Layer::on_event(engine::event& event) {
	if (!(attach_transition_in_progress || detach_transition_in_progress)) {
		if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
			auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);

			// When clicked, if hovering over a button, call it's onClick() function. Otherwise enter state to start dragging.
			if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
				if (m_hover) m_button_manager->check_confirm();
				else {
					m_clicked = true;
					m_start_mouse_position = m_current_mouse_position;
				}
			}
		} else if (event.event_type() == engine::event_type_e::mouse_button_released) {
			auto& e = dynamic_cast<engine::mouse_button_released_event&>(event);

			// If dragging has stopped, record the ending drag rotation model by adding it to the model rotation matrix.
			if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
				if (!m_hover) {
					m_clicked = false;
					model_rotation = m_drag_rotation * model_rotation;
					m_drag_rotation = glm::mat4(1.0f);
				}
			}
		} else if (event.event_type() == engine::event_type_e::mouse_moved) {
			auto& e = dynamic_cast<engine::mouse_moved_event&>(event);

			// Check if button has been hovered over.
			int result = m_button_manager->check_hover_text(e.x(), e.y());

			// If mouse is hovering over item...
			if (result > -1) m_hover = true;
			else m_hover = false;

			m_current_mouse_position = glm::vec3(e.x(), e.y(), 0.0f);

			// If moving the mouse while clicking, append onto the drag rotation matrix.
			// Code adapted from: https://stackoverflow.com/questions/62468005/how-to-implement-interactive-rotation-operations-in-a-decent-way
			if (m_clicked) {
				glm::vec2 drag_vec = glm::vec2(e.x() - m_start_mouse_position.x, m_start_mouse_position.y - e.y());
				glm::vec3 axis_vec = glm::normalize(glm::vec3(-drag_vec.y, drag_vec.x, 0.0f));
				float     angle = glm::length(drag_vec) * 2.0f / (float)engine::application::window().height() / engine::PI * 2.0f;
				m_drag_rotation = glm::rotate(glm::mat4(1.0f), angle, axis_vec);
			}
		} else if (event.event_type() == engine::event_type_e::mouse_scrolled) {
			auto& e = dynamic_cast<engine::mouse_scrolled_event&>(event);

			// Increase/Decrease scale when scrolling mouse.
			m_scale += e.y_offset() * 0.1f;
			m_scale = std::clamp(m_scale, 3.5f, 7.5f);
		} else if (event.event_type() == engine::event_type_e::key_pressed) {
			auto& e = dynamic_cast<engine::key_pressed_event&>(event);

			// If pressing R while model was rotated by the player, reset the model's rotation & scale to the original rotation & scale.
			if (e.key_code() == engine::key_codes::KEY_R && model_rotation != glm::mat4_cast(m_rotations.back())) {
				m_rotation_progress = 0.0f;
				m_item_reset_transformation_in_progress = true;
				m_audio_manager->play("ui_click");
			}
		}
	}
}
