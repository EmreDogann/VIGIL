#include "Level2_Layer.h"
#include "engine/events/key_event.h"

// Reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <gl/GL.h>

#include "Inventory_Layer.h"
#include "Death_Layer.h"
#include "Pickup_Layer.h"

std::vector<std::string> split(const std::string& s, char delim);

void Level2_layer::set_default_shader_parameters() {
	auto shader = engine::renderer::shaders_library()->get("mesh");

	// Set default mesh shader parameters.
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gMatSpecularIntensity", 0.1f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpecularPower", 10.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gNumPointLights", static_cast<int32_t>(m_point_lights.size()));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gNumSpotLights", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", 0.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", 0.0f);
}

Level2_layer::Level2_layer() :
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height(),
	            std::make_shared<engine::fixed_camera_position>(
		            glm::vec3(-1.84798, 1.63582, -1.7652), glm::vec3(0.0393038, 0.78249, -0.158995), false,
		            std::make_shared<engine::bounding_box>(3.5f, 0.3f, 2.0f, glm::vec3(1.0f))), 45.0f),
	m_3d_camera_debug((float)engine::application::window().width(), (float)engine::application::window().height()) {
	// Hide the mouse and lock it inside the window
	engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::application::audio();
	m_audio_manager->play("level2_ambiance");

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	engine::PointLight point_light;
	point_light.Color = glm::vec3(1.0f, 0.95f, 0.95f);
	point_light.AmbientIntensity = 0.4f;
	point_light.DiffuseIntensity = 0.5f;
	point_light.Attenuation.Linear = 0.5f;
	point_light.Attenuation.Exp = 0.01f;
	point_light.Position = glm::vec3(0.0f, 1.0f, 0.0f);
	m_point_lights.push_back(point_light);

	point_light.Position = glm::vec3(0.0f, 1.0f, 3.5f);
	m_point_lights.push_back(point_light);

	// Set the default shader parameters;
	Level2_layer::set_default_shader_parameters();

	// Submit all point lights to the scene.
	for (int i = 0; i < m_point_lights.size(); i++) {
		m_point_lights[i].submit(mesh_shader, i);
	}

	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	// Setup cross-fade transition quad.
	engine::ref<engine::quad>      quad_shape = engine::quad::create({(float)engine::application::window().width() / (float)engine::application::window().height(), 1}, -1);
	engine::game_object_properties quad_props;
	quad_props.position = {0.f, 0.f, -0.1f};
	quad_props.meshes = {quad_shape->mesh()};
	quad_props.textures = {engine::texture_2d::create("assets/textures/transition_image.png", false)};
	quad_props.type = 1;
	transition_image = engine::game_object::create(quad_props);

	// Fill array with 4 rooms.
	m_rooms.push_back(Room());
	m_rooms.push_back(Room());
	m_rooms.push_back(Room());
	m_rooms.push_back(Room());

	// Setup Room Triggers
	m_rooms[0].m_trigger.push_back({std::make_shared<engine::bounding_box>(0.8f, 1.3f, 0.4f, glm::vec3(0.0f, 0.0f, 1.8f)), 1});
	m_rooms[0].m_trigger.push_back({std::make_shared<engine::bounding_box>(0.4f, 1.3f, 0.6f, glm::vec3(-1.9f, 0.0f, 0.0f)), 3});

	m_rooms[1].m_trigger.push_back({m_rooms[0].m_trigger[0].first, 0}); // The same triggers will be used for multiple rooms, hence the sharing of triggers.
	m_rooms[1].m_trigger.push_back({std::make_shared<engine::bounding_box>(0.4f, 1.3f, 0.8f, glm::vec3(1.9f, 0.0f, 3.6f)), 2});

	m_rooms[2].m_trigger.push_back({m_rooms[1].m_trigger[1].first, 1});

	m_rooms[3].m_trigger.push_back({m_rooms[0].m_trigger[1].first, 0});

	// Setup player starting positions for each room.
	m_rooms[0].m_last_player_pos = glm::vec3(0.0f, 0.30f, -1.5f);
	m_rooms[1].m_last_player_pos = glm::vec3(0.0f, 0.30f, 2.25f);
	m_rooms[2].m_last_player_pos = glm::vec3(2.25f, 0.30f, 3.65f);
	m_rooms[3].m_last_player_pos = glm::vec3(-2.30f, 0.30f, 0.0f);

	// Create player
	m_player = Player::create();
	m_player->set_position(m_rooms[m_current_room_index].m_last_player_pos);
	// Rotate player 180 degrees
	m_player->set_forward({0.0f, 0.0f, 1.0f});
	m_player->set_rotation_amount(atan2(m_player->forward().x, m_player->forward().z));

	// Create Enemies and assign them to their respective rooms.
	m_rooms[0].m_enemies.push_back(Enemy::create(glm::vec3(2.45f, 0.30f, -1.39f), glm::vec3(-1.0f, 0.0f, 0.0f)));
	m_rooms[1].m_enemies.push_back(Enemy::create(glm::vec3(-1.19f, 0.30f, 4.92f), glm::vec3(0.0f, 0.0f, -1.0f)));
	m_rooms[1].m_enemies.push_back(Enemy::create(glm::vec3(-3.62, 0.30f, 4.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	m_rooms[3].m_enemies.push_back(Enemy::create(glm::vec3(-4.25f, 0.30f, -1.46f), glm::vec3(0.0f, 0.0f, 1.0f)));

	// Initialize Camera.
	{
		// Add fixed camera positions
		m_3d_camera_debug.position({0.0f, 0.5f, 0.0f});
		// Add fixed camera positions
		std::string   line;
		std::ifstream myfile("assets/fixed_camera_angles_level2.txt");

		struct file_camera {
			glm::vec3 cam_pos;
			glm::vec3 target_pos;
			bool      look_at_player;

			std::shared_ptr<engine::bounding_box> trigger_volume;
		};

		// Vector to store all the camera angles.
		std::vector<file_camera> file_camera_angles;

		// Read text file and extract all camera angles.
		if (myfile.is_open()) {
			while (getline(myfile, line)) {
				file_camera              camera;
				std::vector<std::string> string_elements = split(line, '{');
				std::vector<std::string> cam_info = split(string_elements.at(0), '(');
				std::vector<std::string> position = split(cam_info.at(0), ',');

				// Collect camera info (cam position, target position, look at player)
				camera.cam_pos = glm::vec3(std::stof(position.at(0)), std::stof(position.at(1)), std::stof(position.at(2)));
				position = split(cam_info.at(1), ',');
				camera.target_pos = glm::vec3(std::stof(position.at(0)), std::stof(position.at(1)), std::stof(position.at(2)));
				camera.look_at_player = cam_info.at(2) != "0";

				// Collect trigger volume info (width, height, depth, position)
				cam_info = split(string_elements.at(1), '(');
				position = split(cam_info.at(0), ',');
				glm::vec3 b_box_dimensions{std::stof(position.at(0)), std::stof(position.at(1)), std::stof(position.at(2))};
				position = split(cam_info.at(1), ',');

				camera.trigger_volume = std::make_shared<engine::bounding_box>(b_box_dimensions.x, b_box_dimensions.y, b_box_dimensions.z,
				                                                               glm::vec3(std::stof(position.at(0)) + b_box_dimensions.x / 2.0f, std::stof(position.at(1)),
				                                                                         std::stof(position.at(2)) + b_box_dimensions.z / 2.0f));

				file_camera_angles.push_back(camera);
			}
			myfile.close();
		}

		for (auto cam : file_camera_angles) {
			m_3d_camera.add_camera_position(std::make_shared<engine::fixed_camera_position>(cam.cam_pos, cam.target_pos, cam.look_at_player, cam.trigger_volume));
		}
	}

	// Initialize First Aid Spray
	{
		// Create First Aid Spray primitive game object.
		engine::ref<engine::first_aid_spray> first_aid_spray_shape = engine::first_aid_spray::create(glm::vec3(0.03f), -1);
		engine::game_object_properties       first_aid_spray_props;
		first_aid_spray_props.position = {-1.0f, 0.89f, 1.475f};
		first_aid_spray_props.meshes = {first_aid_spray_shape->mesh()};
		first_aid_spray_props.textures = {engine::texture_2d::create("assets/textures/FirstAidSpray.png", false)};
		first_aid_spray_props.type = 1;
		first_aid_spray_props.tag = "Pickup";
		first_aid_spray_props.is_static = true;
		first_aid_spray_props.bounding_shape = glm::vec3(0.5f);
		m_first_aid_spray = engine::game_object::create(first_aid_spray_props);

		// First Aid Spray Collider
		m_first_aid_spray->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 0.1f, 0.3f, m_first_aid_spray->position()));
		m_first_aid_spray_data.push_back(first_aid_spray_props.position);

		m_rooms[0].m_game_objects.push_back({m_first_aid_spray, {glm::radians(-110.0f), glm::vec3(0.0f, 1.0f, 0.0f)}});

		first_aid_spray_props.position = {-4.4f, 0.74f, 2.5f};
		m_first_aid_spray = engine::game_object::create(first_aid_spray_props);

		// First Aid Spray Collider
		m_first_aid_spray->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 0.1f, 0.3f, m_first_aid_spray->position()));
		m_first_aid_spray_data.push_back(first_aid_spray_props.position);

		m_rooms[1].m_game_objects.push_back({m_first_aid_spray, {glm::radians(-110.0f), glm::vec3(0.0f, 1.0f, 0.0f)}});
	}

	// Initialize Ammo Box
	{
		// Create Ammo Box primitive game object.
		engine::ref<engine::ammo_box>  ammo_box_shape = engine::ammo_box::create(glm::vec3(0.015f), -1);
		engine::game_object_properties ammo_box_props;
		ammo_box_props.position = {-4.93f, 0.32f, 5.15f};
		ammo_box_props.meshes = {ammo_box_shape->mesh()};
		ammo_box_props.textures = {engine::texture_2d::create("assets/textures/AmmoBox.png", false)};
		ammo_box_props.type = 2;
		ammo_box_props.tag = "Pickup";
		ammo_box_props.is_static = true;
		ammo_box_props.bounding_shape = glm::vec3(0.5f);
		m_ammo_box = engine::game_object::create(ammo_box_props);

		// Ammo Box Collider
		m_ammo_box->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 0.1f, 0.3f, m_ammo_box->position()));
		m_ammo_box_data.push_back(ammo_box_props.position);

		m_rooms[1].m_game_objects.push_back({m_ammo_box, {glm::radians(-50.0f), glm::vec3(0.0f, 1.0f, 0.0f)}});

		ammo_box_props.position = {-3.5f, 0.32f, -0.9f};
		m_ammo_box = engine::game_object::create(ammo_box_props);

		// Ammo Box Collider
		m_ammo_box->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 0.1f, 0.3f, m_ammo_box->position()));
		m_ammo_box_data.push_back(ammo_box_props.position);

		m_rooms[3].m_game_objects.push_back({m_ammo_box, {glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f)}});
	}

	// Initialize Gun
	{
		// Load the gun model. Create a gun object. Set its properties.
		engine::ref<engine::model>     gun_pickup = engine::model::create("assets/models/static/beretta/gun.fbx");
		engine::game_object_properties gun_pickup_props;
		gun_pickup_props.meshes = gun_pickup->meshes();
		gun_pickup_props.textures = {engine::texture_2d::create("assets/models/static/beretta/M9_palette.png", false)};
		gun_pickup_props.type = 0;
		gun_pickup_props.tag = "Pickup";

		float gun_scale = 1.f / glm::max(gun_pickup->size().x, glm::max(gun_pickup->size().y, gun_pickup->size().z));
		gun_pickup_props.position = {5.0f, 0.695f, 4.5f};
		gun_pickup_props.rotation_axis = glm::vec3(0.0f, 0.0f, 1.0f);
		gun_pickup_props.rotation_amount = glm::radians(-90.0f);
		gun_pickup_props.scale = glm::vec3(gun_scale) / 6.0f;
		gun_pickup_props.bounding_shape = gun_pickup->size() / 2.f * gun_scale;
		gun_pickup_props.is_static = true;
		engine::ref<engine::game_object> m_gun_pickup = engine::game_object::create(gun_pickup_props);

		m_gun_pickup->add_bounding_box(std::make_shared<engine::bounding_box>(0.5f, 0.1f, 0.5f, m_gun_pickup->position()));

		m_rooms[2].m_game_objects.push_back({m_gun_pickup, {glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)}});
	}

	// Initialize House Model
	{
		// Load the house indoor model.
		const engine::ref<engine::model> house_indoor = engine::model::create("assets/models/static/house_indoor/house_indoor.fbx");
		engine::game_object_properties   house_props;

		house_props.meshes = house_indoor->meshes();
		const float house_scale = 1.f / glm::max(house_indoor->size().x, glm::max(house_indoor->size().y, house_indoor->size().z));
		house_props.position = {0.0f, 0.0f, 0.0f};
		house_props.scale = glm::vec3(house_scale * 11);
		house_props.bounding_shape = house_indoor->size() / 2.f * house_scale;
		house_props.textures = {
			engine::texture_2d::create("assets/models/static/house_indoor/wood.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/keypad.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/toilet.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/carpet.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/paintings.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/cupboard.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/table.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/bed.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/door.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/wall.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/tiles.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/decorations.png", false),
			engine::texture_2d::create("assets/models/static/house_indoor/kitchen.png", false)
		};
		house_props.is_static = true;
		m_house = engine::game_object::create(house_props);
	}

	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create("IMFellDWPicaSC-Regular.ttf");

	m_pathfinding = Pathfinding::create();

	// Load colliders from file.

	{
		std::string   line;
		std::ifstream myfile("assets/obstacles_level2.txt");

		// Read text file and extract all camera angles.
		if (myfile.is_open()) {
			while (getline(myfile, line)) {
				std::vector<std::string> props = split(line, ',');

				m_obstacles.push_back(std::make_shared<engine::bounding_box>(std::stof(props[0]), std::stof(props[1]), std::stof(props[2]),
				                                                             glm::vec3(std::stof(props[3]), std::stof(props[4]), std::stof(props[5]))));
			}
			myfile.close();
		}

		m_pathfinding->update_map(m_obstacles);
	}

	// Give enemies an initial path to the player's starting position of their respective rooms.
	for (auto& room : m_rooms) {
		for (auto& enemy : room.m_enemies) {
			m_current_astar_path = m_pathfinding->solve_aStar(enemy->position(), room.m_last_player_pos);
			enemy->update_path(m_current_astar_path);
		}
	}

	// Being Fade-In transition.
	attach_transition_in_progress = true;
	set_is_layer_active(true);
}

Level2_layer::~Level2_layer() {}

void Level2_layer::on_update(const engine::timestep& time_step) {
	if (m_player->is_pickup_anim_finished()) {
		if (attach_transition_in_progress) {
			attach_scene_transition(time_step);
		} else if (detach_transition_in_progress) {
			detach_scene_transition(time_step);
		}
	}

	m_3d_camera.on_update(time_step, *m_player->bounding_box()[0]);
	m_3d_camera_debug.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, static_cast<double>(time_step));

	const glm::vec3 old_player_pos = m_player->position();

	m_player->on_update(time_step, m_physics_manager);

	// Check for game object collisions in the current room.
	for (auto& gm : m_rooms[m_current_room_index].m_game_objects) {
		// Remove item from list of game objects in current room if that item has been marked for removal.
		if (gm.first != nullptr) {
			if (gm.first->should_remove()) {
				m_rooms[m_current_room_index].m_game_objects.erase(std::remove(m_rooms[m_current_room_index].m_game_objects.begin(), m_rooms[m_current_room_index].m_game_objects.end(), gm),
				                                                   m_rooms[m_current_room_index].m_game_objects.end());
			} else {
				// Don't stop the player on collisions with pickup items.
				if (gm.first->tag() != "Pickup") {
					for (engine::ref<engine::bounding_box> b_box : gm.first->bounding_box()) {
						if (b_box->collision(*(m_player->bounding_box()[0])))
							m_player->set_position(old_player_pos);
					}
				}
			}
		}
	}

	// Check for obstacle (environment) collisions.
	for (engine::ref<engine::bounding_box> b_box : m_obstacles) {
		if (b_box->collision(*(m_player->bounding_box()[0]))) {
			m_player->set_position(old_player_pos);
		}
	}

	// Check for enemy collisions.
	for (auto& enemy : m_rooms[m_current_room_index].m_enemies) {
		if (!enemy->is_dead() && enemy->bounding_box()[0]->collision(*(m_player->bounding_box()[0])))
			m_player->set_position(old_player_pos);
	}

	// Check if enemies have been hit by bullets;
	if (m_player->is_weapon_equipped()) {
		for (auto& enemy : m_rooms[m_current_room_index].m_enemies) {
			if (!enemy->is_dead() && !enemy->is_taking_damage()) {
				for (auto& bullet : m_player->get_bullet_objects()) {
					if (enemy->bounding_box()[0]->collision(*(bullet->bounding_box()[0])))
						enemy->apply_damage(15.0f, m_3d_camera);
				}
			}
		}
	}

	// Update path-finding for all enemies in the current room & call their on_updates.
	m_pathfinding_timer -= time_step;
	for (auto& enemy : m_rooms[m_current_room_index].m_enemies) {
		if (!enemy->is_dead() && m_pathfinding_timer <= 0.0f && m_player->position() != old_player_pos) {
			m_current_astar_path = m_pathfinding->solve_aStar(enemy->position(), m_player->position());
			enemy->update_path(m_current_astar_path);
		}
		enemy->on_update(time_step, *m_player, m_3d_camera);
	}

	if (m_pathfinding_timer <= 0.0f) {
		m_pathfinding_timer = m_pathfinding_update_frequency;
	}

	// Check if player has died.
	if (m_player->is_dead && !(attach_transition_in_progress || detach_transition_in_progress)) {
		detach_transition_in_progress = true;
		on_transition_finish = [this]()
		{
			engine::application::instance().pop_layer_buffer(this);
			engine::application::instance().push_layer_buffer(new Death_Layer());
		};
	}

	// Play player footsteps
	if (m_player->position() != old_player_pos) {
		m_footstep_timer -= time_step;
		if (m_footstep_timer <= 0.0f) {
			m_audio_manager->play_spatialised_sound("wood_footstep" + std::to_string(m_random_footstep_sound(mt)), m_3d_camera.position(), m_player->position());
			m_footstep_timer = m_footstep_timer_cooldown;
		}
	} else {
		m_footstep_timer = m_footstep_timer_cooldown;
	}

	m_audio_manager->update_with_camera(m_3d_camera);
}

void Level2_layer::on_render() {
	engine::render_command::clear_color({0.0f, 0.0f, 0.0f, 1.0f});
	engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(debug_camera ? m_3d_camera_debug : m_3d_camera, mesh_shader);
	m_material->submit(mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", debug_camera ? m_3d_camera_debug.position() : m_3d_camera.position());
	//m_3d_camera.on_render(mesh_shader);	// Uncomment to show fixed camera angle bounding boxes.

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_ALWAYS, 0.0f);

	// Render House
	glm::mat4 house_transform(1.0f);
	house_transform = glm::translate(house_transform, m_house->position());
	house_transform = glm::rotate(house_transform, m_house->rotation_amount(), m_house->rotation_axis());
	house_transform = glm::scale(house_transform, m_house->scale());
	engine::renderer::submit(mesh_shader, house_transform, m_house);

	// Render all game objects in the current room.
	for (auto& gm : m_rooms[m_current_room_index].m_game_objects) {
		if (gm.first->tag() == "Pickup") {
			glDisable(GL_CULL_FACE); // Temporarily disable culling to render both sides of the sprite/quad.
			//std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
		}

		glm::mat4 gm_transform(1.0f);
		gm_transform = glm::translate(gm_transform, gm.first->position());
		gm_transform = glm::rotate(gm_transform, gm.first->rotation_amount(), gm.first->rotation_axis());
		gm_transform = glm::rotate(gm_transform, gm.second.first, gm.second.second); // Apply additional rotations
		gm_transform = glm::scale(gm_transform, gm.first->scale());
		engine::renderer::submit(mesh_shader, gm_transform, gm.first);

		//std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
		glEnable(GL_CULL_FACE);
	}

	// Render player
	m_player->on_render(mesh_shader, m_player);

	// Render all enemies in the current room.
	for (auto& enemy : m_rooms[m_current_room_index].m_enemies) {
		enemy->on_render(mesh_shader, enemy);
	}

	// Render bounding boxes.
	if (m_show_bounding_boxes) {
		m_material->submit(mesh_shader);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
		// Render all obstacle (environment) colliders.
		for (auto obstacle : m_obstacles) {
			if (obstacle == m_debug_selected_obstacle) {
				obstacle->on_render(0.0f, 256.0f, 0.0f, mesh_shader);
			} else {
				obstacle->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
			}
		}

		// Render all game object bounding boxes in the current room.
		for (auto& gm : m_rooms[m_current_room_index].m_game_objects) {
			for (engine::ref<engine::bounding_box> b_box : gm.first->bounding_box()) {
				b_box->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
			}
		}

		// Render all enemy bounding boxes in the current room.
		for (engine::ref<Enemy> enemy : m_rooms[m_current_room_index].m_enemies) {
			for (engine::ref<engine::bounding_box> b_box : enemy->bounding_box()) {
				b_box->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
			}
		}

		if (m_player->is_weapon_equipped()) {
			// Render all game object bounding boxes in the current room.
			for (auto& bullet : m_player->get_bullet_objects()) {
				for (engine::ref<engine::bounding_box> b_box : bullet->bounding_box()) {
					b_box->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
				}
			}
		}

		// Render the player's bounding box.
		m_player->bounding_box()[0]->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	}

	// Render enemy pathfinding.
	if (m_show_pathfinding) {
		m_pathfinding->on_render(mesh_shader);
	}

	engine::renderer::end_scene();

	// Render scene transition (Black screen)
	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	if ((attach_transition_in_progress || detach_transition_in_progress) && m_player->is_pickup_anim_finished()) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void Level2_layer::on_event(engine::event& event) {
	if (!(detach_transition_in_progress || attach_transition_in_progress || !m_player->is_pickup_anim_finished())) m_player->on_event(event, m_physics_manager, m_3d_camera);
	//m_3d_camera.on_event(event, m_3d_camera_debug, *m_player);	// Uncomment to enable fixed camera angle controls.

	if (event.event_type() == engine::event_type_e::key_pressed && !(detach_transition_in_progress || attach_transition_in_progress || !m_player->is_pickup_anim_finished())) {
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_TAB) {
			detach_transition_in_progress = true;
			transition_length = 0.15f;
			transition_timer = transition_length;

			on_transition_finish = [this]()
			{
				is_layer_active = false;
				transition_timer = transition_length;
				engine::application::instance().push_layer_buffer(new Inventory_Layer(m_player));
			};
		} else if (e.key_code() == engine::key_codes::KEY_E) {
			// Check for collisions with pickups in the current room.
			for (auto& gm : m_rooms[m_current_room_index].m_game_objects) {
				if (gm.first->tag() == "Pickup" && gm.first->bounding_box()[0]->collision(*(m_player->bounding_box()[0]))) {
					// Play different pickup animation depending on how far up/down from the ground the item is.
					if (gm.first->position().y <= m_player->position().y + (m_player->scale().y / 2.0f)) {
						m_player->pickup_item(8);
					} else {
						m_player->pickup_item(9);
					}

					detach_transition_in_progress = true;
					transition_length = 0.35f;
					transition_timer = transition_length;

					on_transition_finish = [this, &gm]()
					{
						is_layer_active = false;
						transition_timer = transition_length;
						m_player->unpause_animations();
						engine::application::instance().push_layer_buffer(new Pickup_Layer(m_player, gm.first));

						attach_transition_in_progress = true;
					};
				}
			}

			// Check if player is colliding with any room transition triggers in the current room.
			auto& room = m_rooms[m_current_room_index];
			for (auto& room_trigger : room.m_trigger) {
				if (room_trigger.first->collision(*(m_player->bounding_box()[0]))) {
					room.m_last_player_pos = m_player->position(); // Record the player's last position in the room.
					m_next_room_index = room_trigger.second;       // Set the room index to switch to after the transition is finished.

					detach_transition_in_progress = true;
					transition_length = 1.5f;
					transition_timer = transition_length;
					m_audio_manager->play("door_transition");

					on_transition_finish = [this]()
					{
						m_current_room_index = m_next_room_index;
						m_player->set_position(m_rooms[m_current_room_index].m_last_player_pos); // Move player into new room.

						transition_timer = transition_length;
						attach_transition_in_progress = true;
					};
				}
			}

		} else if (e.key_code() == engine::key_codes::KEY_Z) {
			std::cout << "Camera Position: " << (debug_camera ? m_3d_camera_debug.position() : m_3d_camera.position()) << std::endl;
		} else if (e.key_code() == engine::key_codes::KEY_R) {
			std::cout << "Player Position: " << m_player->position() << std::endl;
		} else if (e.key_code() == engine::key_codes::KEY_Q) {
			debug_camera = !debug_camera;
		} else if (e.key_code() == engine::key_codes::KEY_T) {
			m_show_bounding_boxes = !m_show_bounding_boxes;
		} else if (e.key_code() == engine::key_codes::KEY_Y) {
			m_show_pathfinding = !m_show_pathfinding;
		}

		// Uncomment to enable controls to create obstacles for path-finding.
		{
			//	if (e.key_code() == engine::key_codes::KEY_ENTER) {
			//		// Create obstacle. Round position to 1 decimal place.
			//		auto b_box = std::make_shared<engine::bounding_box>(1.0f, 0.5f, 1.0f, glm::vec3(
			//			                                                    static_cast<float>(static_cast<int>(m_3d_camera_debug.position().x * 10.0f)) / 10.0f, 0.0f,
			//			                                                    static_cast<float>(static_cast<int>(m_3d_camera_debug.position().z * 10.0f)) / 10.0f));

			//		m_obstacles.push_back(b_box);
			//		m_debug_selected_obstacle = b_box;

			//		m_pathfinding->update_map(m_obstacles);
			//	} else if (e.key_code() == engine::key_codes::KEY_BACKSPACE) {
			//		// Find and Delete obstacle.
			//		auto it = m_obstacles.begin();
			//		while (it != m_obstacles.end()) {
			//			if ((*it)->collision(engine::bounding_box(0.1f, 0.1f, 0.1f, glm::vec3(m_3d_camera_debug.position().x, 0.0f, m_3d_camera_debug.position().z)))) {
			//				if (*it == m_debug_selected_obstacle) {
			//					it = m_obstacles.erase(it);
			//					if (it == m_obstacles.end()) {
			//						m_debug_selected_obstacle = nullptr;
			//					} else {
			//						m_debug_selected_obstacle = *it;
			//					}
			//				} else {
			//					it = m_obstacles.erase(it);
			//				}

			//				m_pathfinding->update_map(m_obstacles);
			//			} else {
			//				++it;
			//			}
			//		}
			//	} else if (e.key_code() == engine::key_codes::KEY_RIGHT_SHIFT) {
			//		// Select colliding bounding box.
			//		for (auto b_box : m_obstacles) {
			//			if (b_box->collision(engine::bounding_box(0.05f, 0.05f, 0.05f, glm::vec3(m_3d_camera_debug.position().x, 0.0f, m_3d_camera_debug.position().z)))) {
			//				m_debug_selected_obstacle = b_box;
			//			}
			//		}
			//	} else if (e.key_code() == engine::key_codes::KEY_L) {
			//		bounding_box_info b_box_info{};
			//		m_debug_selected_obstacle->get(b_box_info.position, b_box_info.width, b_box_info.height, b_box_info.depth);

			//		// Round decimals to remove rounding errors.
			//		glm::vec3 pos = {
			//			std::round((b_box_info.position.x + (b_box_info.width / 2.0f)) * 100.0f) / 100.0f, 0.0f,
			//			std::round((b_box_info.position.z + (b_box_info.depth / 2.0f)) * 100.0f) / 100.0f
			//		};

			//		std::cout << b_box_info.width << ','
			//			<< b_box_info.height << ',' <<
			//			b_box_info.depth << ',' << std::to_string(pos.x) << ',' << std::to_string(pos.y) << ',' << std::to_string(
			//				pos.z) << '\n';
			//	}
			//		//else if (e.key_code() == engine::key_codes::KEY_4) {
			//		//	// Save obstacle info to file.
			//		//	bounding_box_info b_box_info{};
			//		//	std::ofstream     file;
			//		//	file.open("assets/obstacles_level2.txt", std::ofstream::trunc);

			//		//	for (auto b_box : m_obstacles) {
			//		//		b_box->get(b_box_info.position, b_box_info.width, b_box_info.height, b_box_info.depth);

			//		//		// Round decimals to remove rounding errors.
			//		//		glm::vec3 pos = {
			//		//			std::round((b_box_info.position.x + (b_box_info.width / 2.0f)) * 100.0f) / 100.0f, 0.0f,
			//		//			std::round((b_box_info.position.z + (b_box_info.depth / 2.0f)) * 100.0f) / 100.0f
			//		//		};

			//		//		file << b_box_info.width << ','
			//		//			<< b_box_info.height << ',' <<
			//		//			b_box_info.depth << ',' << std::to_string(pos.x) << ',' << std::to_string(pos.y) << ',' << std::to_string(
			//		//				pos.z) << '\n';
			//		//	}
			//		//	file.close();
			//		//}
			//	else if (m_debug_selected_obstacle) {
			//		// If a bounding box is selected
			//		bounding_box_info b_box_info{};
			//		m_debug_selected_obstacle->get(b_box_info.position, b_box_info.width, b_box_info.height, b_box_info.depth);
			//		b_box_info.position += glm::vec3(b_box_info.width / 2.0f, 0.0f, b_box_info.depth / 2.0f);

			//		if (e.key_code() == engine::key_codes::KEY_UP) {
			//			// Move Backward
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.0f, 0.0f, -0.1f));

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_DOWN) {
			//			// Move Forward
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.0f, 0.0f, 0.1f));

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_LEFT) {
			//			// Move Left
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(-0.1f, 0.0f, 0.0f));

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_RIGHT) {
			//			// Move Right
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height, b_box_info.depth, b_box_info.position + glm::vec3(0.1f, 0.0f, 0.0f));

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_F1) {
			//			// Increase Width
			//			m_debug_selected_obstacle->set_box(b_box_info.width + 0.2f, b_box_info.height, b_box_info.depth, b_box_info.position);

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_F2) {
			//			// Increase Height
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height + 0.2f, b_box_info.depth, b_box_info.position);

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_F3) {
			//			// Increase Depth
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height, b_box_info.depth + 0.2f, b_box_info.position);

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_1) {
			//			// Decrease Width
			//			m_debug_selected_obstacle->set_box(b_box_info.width - 0.2f, b_box_info.height, b_box_info.depth, b_box_info.position);

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_2) {
			//			// Decrease Height
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height - 0.2f, b_box_info.depth, b_box_info.position);

			//			m_pathfinding->update_map(m_obstacles);
			//		} else if (e.key_code() == engine::key_codes::KEY_3) {
			//			// Decrease Depth
			//			m_debug_selected_obstacle->set_box(b_box_info.width, b_box_info.height, b_box_info.depth - 0.2f, b_box_info.position);

			//			m_pathfinding->update_map(m_obstacles);
			//		}
			//	}
		}
	}
}

void Level2_layer::attach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = transition_timer / transition_length;

	if (transition_timer <= 0.0f) {
		attach_transition_in_progress = false;

		on_transition_finish();
		on_transition_finish = []()
			{}; // Reset the on_transition_finish callback to empty otherwise it will constantly be transitioning.

		transition_timer = transition_length;
		m_transparency = 0.0f;
	}
}

void Level2_layer::detach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = 1.f - transition_timer / transition_length;

	if (transition_timer <= 0.0f) {
		detach_transition_in_progress = false;
		on_transition_finish();
		on_transition_finish = []()
			{}; // Reset the on_transition_finish callback to empty otherwise it will constantly be transitioning.
	}
}
