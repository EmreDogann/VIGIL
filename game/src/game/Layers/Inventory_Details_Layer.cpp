#include "Inventory_Details_Layer.h"

#include <gl/GL.h>

#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"

void Inventory_Details_Layer::set_default_shader_parameters() {
	auto shader = engine::renderer::shaders_library()->get("mesh");

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gMatSpecularIntensity", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_on", false);
}


Inventory_Details_Layer::Inventory_Details_Layer(glm::vec3 origin, engine::orthographic_camera& camera, engine::ref<Pickup>& item_ref, Inventory_Layer* inventory_layer) :
	origin(origin),
	m_2d_camera(camera),
	m_item(item_ref),
	inventory_layer(inventory_layer) {
	// Hide the mouse and lock it inside the window
	engine::application::window().show_mouse_cursor();

	// Initialise the shaders.
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Set directional light parameters.
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.1f;
	m_directionalLight.DiffuseIntensity = 1.0f;
	m_directionalLight.Direction = glm::vec3(0.0f, -1.0f, -0.75f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
	                                                                       glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
	                                                                                  (float)engine::application::window().height()));

	// Set default mesh shader parameters.
	Inventory_Details_Layer::set_default_shader_parameters();

	// Decide text to display and the functionality of the buttons.
	if (m_item->get_object()->type() == 0) {
		// Handgun
		m_available_options.push_back({
			m_item->is_equipped() ? "Unequip" : "Equip", [this, inventory_layer, item_ref]()
			{
				if (!m_item->is_equipped()) {
					inventory_layer->equip_weapon(item_ref);
					engine::application::instance().pop_layer_buffer(this);
				} else {
					inventory_layer->unequip_weapon(item_ref);
					engine::application::instance().pop_layer_buffer(this);
				}
			}
		});
	} else if (m_item->get_object()->type() == 1) {
		// First Aid Spray
		m_available_options.push_back({
			"Use", [this, inventory_layer, item_ref]()
			{
				inventory_layer->heal_player(item_ref);
				inventory_layer->removeFromInventory(item_ref);
				engine::application::instance().pop_layer_buffer(this);
			}
		});
	} else if (m_item->get_object()->type() == 2) {
		// Handgun Ammo
		m_available_options.push_back({
			"Combine", [this, inventory_layer]()
			{
				inventory_layer->start_combine_items();
				engine::application::instance().pop_layer_buffer(this);
			}
		});
	}

	// If clicked, load examine layer.
	m_available_options.push_back({
		"Examine", [this, inventory_layer]()
		{
			inventory_layer->examine_item();
			engine::application::instance().pop_layer_buffer(this);
		}
	});

	// If clicked, delete item.
	m_available_options.push_back({
		"Discard", [this, item_ref, inventory_layer]()
		{
			if (item_ref->is_equipped()) inventory_layer->unequip_weapon(item_ref); // If an equipped weapon is being discarded, unequip it before discarding.
			inventory_layer->removeFromInventory(item_ref);
			engine::application::instance().pop_layer_buffer(this);
		}
	});

	m_directionalLight.submit(mesh_shader);

	// Create options background image.
	engine::game_object_properties quad_props;
	quad_props.scale = glm::vec3(2.0f, 1.0f, 1.0f) / 40.0f;
	quad_props.position = {origin.x + 0.f + quad_props.scale.x, origin.y + 0.f, 0.3f}; // Move it to the the position of the mouse in world space and shift it right a little bit.
	quad_props.meshes = {engine::quad::create({(float)engine::application::window().width() / (float)engine::application::window().height(), 1}, -1)->mesh()};
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Inventory-Options.png", false)};
	quad_props.type = 1;
	options_images.push_back(engine::game_object::create(quad_props));

	// Move down by 1 option image height + a tiny bit more to avoid overlapping bounding boxes.
	quad_props.position.y -= quad_props.scale.y * 2.0f + 0.0001f;
	options_images.push_back(engine::game_object::create(quad_props));

	// Move down by 1 option image height + a tiny bit more to avoid overlapping bounding boxes.
	quad_props.position.y -= quad_props.scale.y * 2.0f + 0.0001f;
	options_images.push_back(engine::game_object::create(quad_props));

	// Create options background image.
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Inventory-Options-Selected.png", false)};
	options_selected_image = engine::game_object::create(quad_props);

	m_text_manager = engine::text_manager::create("ArchivoNarrow-Bold.ttf");

	m_button_manager = ButtonManager::create(m_2d_camera.view_projection_matrix());

	// Create background button images. Create text describing what the button images will do when pressed.
	for (int i = 0; i < options_images.size(); i++) {
		m_button_manager->create_image_button(glm::vec2(options_images[i]->position().x, options_images[i]->position().y),
		                                      glm::vec2(options_images[i]->scale().x * 1.75f, options_images[i]->scale().y), options_images[i], std::vector<engine::ref<engine::game_object>>{options_selected_image},
		                                      m_available_options[i].second);
		m_button_manager->create_text_button(m_available_options[i].first, 0.45f,
		                                     {options_images[i]->position().x, options_images[i]->position().y + (options_images[i]->scale().y / 4.0f)}, {1.f, 1.f, 1.f, 1.0f},
		                                     {1.f, 1.f, 1.f, 1.0f},
		                                     m_text_manager,
		                                     []()
		                                     {});
	}

	is_layer_active = true;
	m_button_manager->set_active(true);

	// Flip y to put mouse coordinates in the expected coordinate system. This is because world_to_screen is expecting the coordinate system to be traditional cartesian coordinate system but with a flipped y.
	glm::vec2 screen_origin = m_button_manager->world_to_screen(glm::vec2(origin.x, -origin.y));
	m_current_active_slot = m_button_manager->check_hover_images(screen_origin.x, screen_origin.y);
}

Inventory_Details_Layer::~Inventory_Details_Layer() {}

void Inventory_Details_Layer::on_update(const engine::timestep& time_step) {
	m_2d_camera.on_update(time_step);
}

void Inventory_Details_Layer::on_render() {
	// Set up shader.
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	glDisable(GL_DEPTH_TEST);

	// Change blending to make buttons less transparent.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);

	// Render button images.
	m_button_manager->on_render_image(mesh_shader);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render button texts.
	m_button_manager->on_render_text(text_shader);

	glEnable(GL_DEPTH_TEST);

	engine::renderer::end_scene();
}

void Inventory_Details_Layer::on_event(engine::event& event) {
	if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		// If mouse is clicked while it is not hovering over one of the options, register that as the player wanting to go back to the inventory.
		if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
			if (m_current_active_slot > -1) m_button_manager->check_confirm();
			else engine::application::instance().pop_layer_buffer(this);
		}
	} else if (event.event_type() == engine::event_type_e::mouse_moved) {
		auto& e = dynamic_cast<engine::mouse_moved_event&>(event);
		// Check if button has been hovered over.
		m_current_active_slot = m_button_manager->check_hover_images(e.x(), e.y());
	}

	if (event.event_type() == engine::event_type_e::key_pressed) {
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		// If key is pressed while it is not hovering over one of the options, register that as the player wanting to go back to the inventory.
		if (m_current_active_slot == -1) engine::application::instance().pop_layer_buffer(this);
	}
}
