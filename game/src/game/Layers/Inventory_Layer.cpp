#include "Inventory_Layer.h"

#include <gl/GL.h>

#include "MainMenu_Layer.h"
#include "Controls_Layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include "Inventory_Details_Layer.h"
#include "Inventory_Examine_Layer.h"
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include "engine/mouse_buttons_codes.h"

void Inventory_Layer::set_default_shader_parameters() {
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


Inventory_Layer::Inventory_Layer(engine::ref<Player>& player_ref) :
	m_player(player_ref),
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_material(engine::material::create(
		0.0f, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(0.0f),
		1.0f)) {
	// Hide the mouse and lock it inside the window
	engine::application::window().show_mouse_cursor();

	// Get global audio manager. Play ui reveal sound effect.
	m_audio_manager = engine::application::audio();
	m_audio_manager->play("ui_reveal1");

	// Initialise the shaders.
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Set directional light parameters.
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.1f;
	m_directionalLight.DiffuseIntensity = 0.75f;
	m_directionalLight.Direction = glm::vec3(0.0f, -1.0f, -0.75f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
	                                                                       glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
	                                                                                  (float)engine::application::window().height()));

	// Set default mesh shader parameters.
	Inventory_Layer::set_default_shader_parameters();

	m_directionalLight.submit(mesh_shader);

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

	// Initialize quad prop for the inventory slot button images.
	quad_props.meshes = {engine::quad::create({1, 1}, -1)->mesh()};
	quad_props.position = {0.0f, 0.0f, -0.05f};
	quad_props.scale = glm::vec3(0.15f);
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/InventorySlot.png", false)};

	// Spacing between inventory slots.
	float spacing = 0.03f;

	// Create a game object for each inventory cell and add it to a vector.
	std::vector<engine::ref<Pickup>>& player_inventory = m_player->get_inventory();
	for (int x = 1; x >= 0; x--) {
		for (int y = 0; y < 4; y++) {
			quad_props.position = glm::vec3(-0.51f + (y * ((quad_props.scale.x * 2.0f) + spacing)), 0.075f + x * ((quad_props.scale.y * 2.0f) + spacing), quad_props.position.z);
			// If the current inventory slot exceeds the array of items in the player's inventory, substitute with null pointer. Flip x so that 1 becomes 0 and 0 becomes 1.
			inventory_slots.push_back(InventorySlot(engine::game_object::create(quad_props), nullptr));
		}
	}

	// Fill inventory slots with items from player's inventory.
	for (auto& item : player_inventory) {
		inventory_slots[item->get_inventory_position()].item = item;
	}

	// Create selected inventory slot image.
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/InventorySlot-Selected.png", false)};
	inventory_slot_selected_image = engine::game_object::create(quad_props);

	// Create red selected inventory slot image for combining items.
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/InventorySlot-CombineRed.png", false)};
	inventory_slot_combine_first_image = engine::game_object::create(quad_props);

	// Create green selected inventory slot image for combining items.
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/InventorySlot-CombineGreen.png", false)};
	inventory_slot_combine_second_image = engine::game_object::create(quad_props);

	// Create health bar images.
	quad_props.position = {0.03f, -0.40f, 0.1f};
	quad_props.scale = glm::vec3(3.3f, 1.0f, 1.0f) / 5.5f;
	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Health-Fine.png", false)};
	health_bar_images.push_back(engine::game_object::create(quad_props));

	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Health-Caution.png", false)};
	health_bar_images.push_back(engine::game_object::create(quad_props));

	quad_props.textures = {engine::texture_2d::create("assets/textures/UI/Health-Danger.png", false)};
	health_bar_images.push_back(engine::game_object::create(quad_props));

	m_text_manager = engine::text_manager::create("ArchivoNarrow-Bold.ttf");
	m_text_manager_alt = engine::text_manager::create("ArchivoNarrow-Regular.ttf");

	m_button_manager = ButtonManager::create(m_2d_camera.view_projection_matrix());

	// Create button images for the inventory slots.
	std::vector<engine::ref<engine::game_object>> selected_images_vec = {inventory_slot_selected_image, inventory_slot_combine_first_image, inventory_slot_combine_second_image};
	for (int i = 0; i < inventory_slots.size(); i++) {
		m_button_manager->create_image_button(glm::vec2(inventory_slots[i].slot_image->position().x, inventory_slots[i].slot_image->position().y),
		                                      glm::vec2(inventory_slots[i].slot_image->scale().x, inventory_slots[i].slot_image->scale().y), inventory_slots[i].slot_image, selected_images_vec, [i]()
		                                      {});
	}

	// Create text buttons at the top.
	m_button_manager->create_text_button("INVENTORY", 0.9f, {0.0f, 0.63f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 0.5f}, m_text_manager, [this]()
	                                     {}
	);
	m_button_manager->create_text_button("QUIT", 0.65f, {0.59f, 0.625f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager_alt, [this]()
	                                     {
		                                     m_button_manager->set_active(false);
		                                     m_audio_manager->stop_all();
		                                     on_transition_finish = [this]()
		                                     {
			                                     engine::application::instance().pop_layer_buffer(this);
			                                     engine::application::instance().push_layer_buffer(new MainMenu_Layer());
		                                     };
		                                     detach_transition_in_progress = true;
	                                     }
	);
	m_button_manager->create_text_button("HELP", 0.65f, {-0.60f, 0.625f}, {1.f, 1.f, 1.f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, m_text_manager_alt, [this]()
	                                     {
		                                     m_button_manager->set_active(false);
		                                     m_clicked = false;

		                                     on_transition_finish = []()
		                                     {
			                                     engine::application::instance().push_layer_buffer(new Controls_Layer());
		                                     };
		                                     detach_transition_in_progress = true;
	                                     }
	);

	// Start Fade-In transition
	attach_transition_in_progress = true;
	is_layer_active = true;
}

Inventory_Layer::~Inventory_Layer() {}

// If function called, load inventory examine layer.
void Inventory_Layer::examine_item() {
	m_clicked = false;
	m_button_manager->set_active(false);
	detach_transition_in_progress = true;

	on_transition_finish = [this]()
	{
		engine::application::instance().push_layer_buffer(new Inventory_Examine_Layer(inventory_slots[m_current_active_slot].item->get_object()));
	};
}

// Enter item combination mode.
void Inventory_Layer::start_combine_items() {
	m_combining = true;
	m_combine_items.first = m_current_active_slot;
}

void Inventory_Layer::combine_items() {
	// If the second item isn't itself and and an actual item was selected.
	if (m_combine_items.second > -1 && m_combine_items.first != m_combine_items.second && inventory_slots[m_combine_items.second].item != nullptr) {
		engine::ref<Pickup>& item_1 = inventory_slots[m_combine_items.first].item;
		engine::ref<Pickup>& item_2 = inventory_slots[m_combine_items.second].item;

		// If combining handgun ammo with a handgun.
		if (item_1->get_object()->type() == 2 && item_2->get_object()->type() == 0) {
			// If the handgun is not already full.
			if (!item_2->at_max_quantity()) {
				int refill_amount_needed = item_2->get_max_quantity() - item_2->get_quantity();
				int refill_amount_available = item_1->get_quantity();

				// Remove the handgun ammo if all it's bullets have been used, otherwise reduce it's bullet count to the appropriate amount.
				if (refill_amount_available - refill_amount_needed <= 0) {
					// Refill handgun's capacity.
					item_2->set_quantity(item_2->get_quantity() + refill_amount_available);
					removeFromInventory(m_combine_items.first);
				} else {
					item_1->set_quantity(refill_amount_available - refill_amount_needed);
					// Refill handgun's capacity.
					item_2->set_quantity(item_2->get_quantity() + refill_amount_needed);
				}
			}
		}
	}
	m_combining = false;
}

void Inventory_Layer::on_update(const engine::timestep& time_step) {
	if (attach_transition_in_progress) {
		attach_scene_transition(time_step);
	} else if (detach_transition_in_progress) {
		detach_scene_transition(time_step);
	}

	m_2d_camera.on_update(time_step);
}

void Inventory_Layer::on_render() {
	engine::render_command::clear_color({0.f, 0.f, 0.f, 1.0f});
	engine::render_command::clear();

	// Set up shader.
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_ALWAYS, 0.0f);

	// Render background image.
	engine::renderer::submit(mesh_shader, background_image);

	// Render health bar.
	if (m_player->get_health() > 30 && m_player->get_health() <= 75) {
		engine::renderer::submit(mesh_shader, health_bar_images[1]);
	} else if (m_player->get_health() <= 30) {
		engine::renderer::submit(mesh_shader, health_bar_images[2]);
	} else {
		engine::renderer::submit(mesh_shader, health_bar_images[0]);
	}

	// Render buttons
	m_button_manager->on_render_text(text_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind(); // Rebind mesh shader other it will try to use text_shader to render everything else.

	// Render the inventory slots differently depending on if in combination mode or not.
	if (m_combining) {
		std::vector<ButtonImage> button_images = m_button_manager->get_button_images();
		for (int i = 0; i < button_images.size(); i++) {
			if (i == m_combine_items.first) {
				m_button_manager->on_render_image_single(mesh_shader, button_images[i], 1);
			} else if (i == m_combine_items.second) {
				m_button_manager->on_render_image_single(mesh_shader, button_images[i], 2);
			} else {
				m_button_manager->on_render_image_single(mesh_shader, button_images[i], 0);
			}
		}
	} else {
		m_button_manager->on_render_image(mesh_shader);
	}

	glDisable(GL_CULL_FACE);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);

	for (auto& slot : inventory_slots) {
		if (slot.item != nullptr) {
			// If item is being clicked and dragged, render it so that it follows the mouse cursor.
			if (m_current_active_slot > -1 && &slot == &inventory_slots[m_current_active_slot] && m_drag) {
				engine::ref<engine::game_object> item_object = slot.item->get_object();

				// Render game object at mouse position
				glm::mat4 item_transform(1.0f);
				item_transform = glm::translate(item_transform, slot.item->get_object()->position() + m_current_world_mouse_pos);
				item_transform = glm::rotate(item_transform, item_object->rotation_amount(), item_object->rotation_axis());
				item_transform = glm::scale(item_transform, item_object->scale());
				engine::renderer::submit(mesh_shader, item_transform, item_object);

			} else {
				engine::ref<engine::game_object> item_object = slot.item->get_object();

				// Render game object at inventory slot position
				glm::mat4 item_transform(1.0f);
				item_transform = glm::translate(item_transform, slot.item->get_object()->position() + glm::vec3(slot.slot_image->position().x, slot.slot_image->position().y, 0.1f));
				item_transform = glm::rotate(item_transform, item_object->rotation_amount(), item_object->rotation_axis());
				item_transform = glm::scale(item_transform, item_object->scale());
				engine::renderer::submit(mesh_shader, item_transform, item_object);

				// Render associated text (magazine count, quantity, etc).
				// Get text dimensions.
				glm::vec4 text_dimensions;
				glm::vec2 text_pos;
				m_text_manager->get_text_dimensions(std::to_string(slot.item->get_quantity()), 0.5f, text_dimensions);

				// Render item quantity text if the item is not empty. Don't render quantity for first aid spray.
				if (slot.item->get_quantity() > 0 && slot.item->get_object()->type() != 1) {
					// Get the text position in screen coordinates and compensate for the screen space length of the text.
					text_pos = m_button_manager->world_to_screen(slot.slot_image->position() + glm::vec3(slot.slot_image->scale().x - 0.04f, -slot.slot_image->scale().y + 0.04f, 0.0f));
					text_pos.x -= text_dimensions.z;
					m_text_manager->render_text(text_shader, std::to_string(slot.item->get_quantity()), text_pos.x, text_pos.y, 0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}

				// If item is equipped, render text to signify the user of this.
				if (slot.item->is_equipped()) {
					// Get the text position in screen coordinates and compensate for the screen space length of the text.
					text_pos = m_button_manager->world_to_screen(slot.slot_image->position() + glm::vec3(-slot.slot_image->scale().x + 0.06f, -slot.slot_image->scale().y + 0.04f, 0.0f));
					text_pos.x -= text_dimensions.z;
					m_text_manager->render_text(text_shader, "Equipped", text_pos.x, text_pos.y, 0.4f, glm::vec4(0.0f, 0.3f, 0.0f, 1.0f));
				}

				// Render text description of active item.
				if (m_current_active_slot > -1 && &slot == &inventory_slots[m_current_active_slot]) {
					m_text_manager->get_text_dimensions(slot.item->get_description(), 0.75f, text_dimensions);
					text_pos = m_button_manager->world_to_screen(glm::vec3(0.0f, -0.175f, 0.1f));
					text_pos.x -= text_dimensions.z / 2.0f;
					m_text_manager->render_text(text_shader, slot.item->get_description(), text_pos.x, text_pos.y, 0.75f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
				std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind(); // Rebind mesh shader other it will try to use text_shader to render everything else.
			}
		}
	}

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	glEnable(GL_CULL_FACE);

	// Render transition image.
	if (attach_transition_in_progress || detach_transition_in_progress) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void Inventory_Layer::on_event(engine::event& event) {
	// Only process events if layer is not in the middle of transitioning.
	if (!(detach_transition_in_progress || attach_transition_in_progress)) {
		if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
			auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
			if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
				if (m_combining) {
					combine_items();
				} else {
					m_clicked = true;
					m_button_manager->check_confirm();
				}
			} else if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_2 && m_current_active_slot > -1 && inventory_slots[m_current_active_slot].item != nullptr && !m_combining) {
				m_clicked = false;
				is_layer_frozen = true;
				transition_timer = 0.0f;

				engine::application::instance().push_layer_buffer(new Inventory_Details_Layer(m_current_world_mouse_pos, m_2d_camera, inventory_slots[m_current_active_slot].item, this));
			}
		} else if (event.event_type() == engine::event_type_e::mouse_button_released) {
			auto& e = dynamic_cast<engine::mouse_button_released_event&>(event);
			if (e.mouse_button() == engine::mouse_button_codes::MOUSE_BUTTON_1) {
				if (m_drag) {
					m_drag = false;
					if (m_next_active_slot > -1) {
						// Save new position in Pickup object. This will used when the inventory layer is loaded again.
						inventory_slots[m_current_active_slot].item->set_inventory_position(m_next_active_slot);

						// Swap next and current items.
						engine::ref<Pickup> temp_item = inventory_slots[m_next_active_slot].item;
						inventory_slots[m_next_active_slot].item = inventory_slots[m_current_active_slot].item;
						inventory_slots[m_current_active_slot].item = temp_item;

						m_current_active_slot = m_next_active_slot;
					}
				}
				m_clicked = false;
			}
		} else if (event.event_type() == engine::event_type_e::mouse_moved) {
			auto& e = dynamic_cast<engine::mouse_moved_event&>(event);
			if (m_clicked && m_current_active_slot > -1 && inventory_slots[m_current_active_slot].item != nullptr && !m_combining) {
				// This conditional being true means the user is clicking and dragging the mouse.
				m_drag = true;

				m_button_manager->check_hover_text(e.x(), e.y());
				m_next_active_slot = m_button_manager->check_hover_images(e.x(), e.y());
			} else {
				m_button_manager->check_hover_text(e.x(), e.y());
				m_current_active_slot = m_button_manager->check_hover_images(e.x(), e.y());
				if (m_combining) m_combine_items.second = m_current_active_slot;
			}

			// Record the mouse position in world space.
			m_current_world_mouse_pos = m_button_manager->screen_to_world(glm::vec3(e.x(), e.y(), 0.1f));
			m_current_world_mouse_pos.y *= -1;
		}

		if (event.event_type() == engine::event_type_e::key_pressed) {
			auto& e = dynamic_cast<engine::key_pressed_event&>(event);
			if (!m_combining) {
				if (e.key_code() == engine::key_codes::KEY_TAB) {
					detach_transition_in_progress = true;
					engine::application::window().hide_mouse_cursor();

					on_transition_finish = [this]()
					{
						engine::application::instance().pop_layer_buffer(this);
					};
				}
			} else {
				m_combining = false;
			}
		}
	}
}

void Inventory_Layer::attach_scene_transition(const engine::timestep& time_step) {
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

void Inventory_Layer::detach_scene_transition(const engine::timestep& time_step) {
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
