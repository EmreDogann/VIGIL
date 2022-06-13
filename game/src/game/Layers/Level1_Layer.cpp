#include "Level1_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtx/rotate_vector.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include <gl/GL.h>

#include "Level2_layer.h"

// Reading a text file
#include <iostream>
#include <fstream>
#include <string>

#include "Inventory_Layer.h"

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> result;
	std::stringstream        ss(s);
	std::string              item;

	while (getline(ss, item, delim)) {
		if (item.size() > 0) {
			result.push_back(item);
		}
	}

	return result;
}

void Level1_layer::set_default_shader_parameters() {
	auto shader = engine::renderer::shaders_library()->get("mesh");

	// Set default mesh shader parameters.
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gMatSpecularIntensity", 0.1f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_colour", glm::vec3{0.0f, 0.0f, 0.0f});
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_factor_type", 1);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_start", 0.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("fog_end", 15.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("rho", 0.10f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gNumSpotLights", 1);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", 0.15f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", 0.2f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.Color", glm::vec3(0.95f, 0.95f, 1.0f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(glm::vec3(0.0f)));
}

Level1_layer::Level1_layer() :
	m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height(),
	            std::make_shared<engine::fixed_camera_position>(
		            glm::vec3(-1.72505f, 1.36913f, 32.7749f), glm::vec3(-0.442367f, 1.17067f, 35.2284f), false,
		            std::make_shared<engine::bounding_box>(3.5f, 0.8f, 2.0f, glm::vec3(1.0f))), 40.0f),
	m_3d_camera_debug((float)engine::application::window().width(), (float)engine::application::window().height()) {
	// Hide the mouse and lock it inside the window
	engine::application::window().hide_mouse_cursor();

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(0.95f, 0.95f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.15f;
	m_directionalLight.DiffuseIntensity = 0.2f;

	m_spotLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_spotLight.AmbientIntensity = 0.5f;
	m_spotLight.DiffuseIntensity = 0.5f;
	m_spotLight.Cutoff = 0.85f;

	Level1_layer::set_default_shader_parameters();

	m_directionalLight.submit(mesh_shader);

	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
	                                      glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	// Create player
	m_player = Player::create();

	m_player->set_position(glm::vec3(-0.3f, 0.5f, 38.5f));

	// Rotate player 180 degrees
	m_player->set_forward(glm::rotate(m_player->forward(), glm::radians(180.0f), m_player->rotation_axis()));
	m_player->set_rotation_amount(atan2(m_player->forward().x, m_player->forward().z));

	engine::ref<engine::quad> quad_shape = engine::quad::create({(float)engine::application::window().width() / (float)engine::application::window().height(), 1}, -1);
	// Initialize Transition Image
	{
		engine::game_object_properties quad_props;
		quad_props.position = {0.f, 0.f, -0.1f};
		quad_props.meshes = {quad_shape->mesh()};
		quad_props.textures = {engine::texture_2d::create("assets/textures/transition_image.png", false)};
		quad_props.type = 1;
		quad_props.is_static = true;
		transition_image = engine::game_object::create(quad_props);
	}

	// Initialize Mansion Image
	{
		engine::ref<engine::quad>      quad_mansion_shape = engine::quad::create({1, 1}, -1);
		engine::game_object_properties quad_mansion_props;
		quad_mansion_props.position = {0.0f, 9.0f, -5.0f};
		quad_mansion_props.scale = glm::vec3{10};
		quad_mansion_props.meshes = {quad_shape->mesh()};
		quad_mansion_props.textures = {engine::texture_2d::create("assets/textures/mansionOutdoor.png", false)};
		quad_mansion_props.type = 1;
		quad_mansion_props.is_static = true;
		m_mansion = engine::game_object::create(quad_mansion_props);
	}

	// Initialize Skybox
	{
		m_skybox = engine::skybox::create(50.f,
		                                  {
			                                  engine::texture_2d::create("assets/textures/skybox/zpos-Gray.png", true),
			                                  engine::texture_2d::create("assets/textures/skybox/xpos-Gray.png", true),
			                                  engine::texture_2d::create("assets/textures/skybox/zneg-Gray.png", true),
			                                  engine::texture_2d::create("assets/textures/skybox/xneg-Gray.png", true),
			                                  engine::texture_2d::create("assets/textures/skybox/ypos-Gray.png", true),
			                                  engine::texture_2d::create("assets/textures/skybox/yneg-Gray.png", true)
		                                  });
	}

	// Initialize Camera
	{
		m_3d_camera_debug.position({0.0f, 1.0f, 35.0f});
		// Add fixed camera positions
		std::string   line;
		std::ifstream myfile("assets/fixed_camera_angles_level1.txt");

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

	// Initialize Height Map
	{
		// Load the height map and its texture. Create a terrain object. Set its properties
		m_heightmap = engine::heightmap::create("assets/textures/DirtyIce-HeightMap.bmp", "assets/textures/DirtyIce.bmp", 100.0f, 100.0f,
		                                        glm::vec3(0.0f, 1.35f, 0.0f), 1.0f);
		engine::game_object_properties terrain_props;
		terrain_props.meshes = {m_heightmap->mesh()};
		terrain_props.textures = {m_heightmap->texture()};
		terrain_props.is_static = true;
		terrain_props.type = 0;
		terrain_props.bounding_shape = glm::vec3(100.f, 0.0f, 100.f);
		terrain_props.restitution = 0.92f;
		m_terrain = engine::game_object::create(terrain_props);
	}

	// Initialize Signs
	{
		// Store alternate textures in array for use in on_render().
		m_sign_textures.push_back(engine::texture_2d::create("assets/models/static/stop_sign/stop_sign_1.png", false));
		m_sign_textures.push_back(engine::texture_2d::create("assets/models/static/stop_sign/stop_sign_2.png", false));
		m_sign_textures.push_back(engine::texture_2d::create("assets/models/static/stop_sign/stop_sign_3.png", false));

		// Load the sign model. Create a sign object. Set its properties.
		engine::ref<engine::model>     stop_sign = engine::model::create("assets/models/static/stop_sign/stop_sign.fbx");
		engine::game_object_properties sign_props;
		sign_props.meshes = stop_sign->meshes();

		// Randomizing the texture here will not matter because it will be overwritten by the on_render for loop.
		sign_props.textures = {m_sign_textures[0]};
		float sign_scale = 1.f / glm::max(stop_sign->size().x, glm::max(stop_sign->size().y, stop_sign->size().z));
		sign_props.position = {0.f, 0.5f, 0.f};
		sign_props.scale = glm::vec3(sign_scale);
		sign_props.bounding_shape = stop_sign->size() / 2.f * sign_scale;
		sign_props.is_static = true;
		m_sign = engine::game_object::create(sign_props);

		m_sign->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 1.0f, 0.3f, m_sign->position()));
		m_signs_data.push_back({
			glm::vec3(1.0f), glm::radians(10.f), glm::radians(20.f),
			m_dist_sign_lifespan(mt), m_dist_sign_texture_index(mt)
		});

		std::string   line;
		std::ifstream myfile("assets/sign_positions.txt");

		// Read text file and extract all sign info.
		if (myfile.is_open()) {
			while (getline(myfile, line)) {
				std::vector<std::string> string_elements = split(line, '\n');
				for (auto element : string_elements) {
					std::vector<std::string> sign_positions = split(element, ',');
					glm::vec3                pos = {std::stof(sign_positions.at(0)), std::stof(sign_positions.at(1)), std::stof(sign_positions.at(2))};

					m_signs_data.push_back({
						pos, std::stof(sign_positions.at(3)), std::stof(sign_positions.at(4)),
						m_dist_sign_lifespan(mt), m_dist_sign_texture_index(mt)
					});
					m_sign->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 1.0f, 0.3f, pos));
				}
			}
			myfile.close();
		}
	}

	// Initialize Trees
	{
		// Store alternate textures in array for use in on_render().
		m_tree_textures.push_back(engine::texture_2d::create("assets/textures/Tree1.png", false));
		m_tree_textures.push_back(engine::texture_2d::create("assets/textures/Tree2.png", false));
		m_tree_textures.push_back(engine::texture_2d::create("assets/textures/Tree3.png", false));

		// Load the tree model. Create a tree object. Set its properties.
		engine::ref<engine::quad>      tree_shape = engine::quad::create({2.f, 2.f}, -1);
		engine::game_object_properties tree_props;
		tree_props.meshes = {tree_shape->mesh()};
		tree_props.textures = {m_tree_textures[0]};
		tree_props.position = {0.f, 0.f, 0.f};
		tree_props.is_static = true;
		tree_props.bounding_shape = glm::vec3(0.1f);
		//tree_props.scale = glm::vec3(1.f);
		m_tree = engine::game_object::create(tree_props);

		// Right path collider
		m_tree->add_bounding_box(std::make_shared<engine::bounding_box>(1.0f, 5.0f, 50.0f, glm::vec3(1.6f, 0.5f, 45.0f)));

		// Left path collider
		m_tree->add_bounding_box(std::make_shared<engine::bounding_box>(1.0f, 5.0f, 50.0f, glm::vec3(-2.4f, 0.5f, 45.0f)));

		// Generate random positions for 'm_tree_count' number of trees and store them along with their randomly selected tree game object.
		for (int i = 0; i < m_tree_count / 2; i++) {
			// Positions for RIGHT side of path.
			glm::vec3 pos = glm::vec3(m_dist_tree_x(mt) - 1, 2.5f, m_dist_tree_z(mt));
			m_trees_data.push_back({pos, m_dist_tree_angle(mt), m_dist_tree_texture_index(mt)});

			// Positions for LEFT side of path.
			pos = glm::vec3(-m_dist_tree_x(mt), 2.5f, m_dist_tree_z(mt));
			m_trees_data.push_back({pos, m_dist_tree_angle(mt), m_dist_tree_texture_index(mt)});
		}
	}

	// Initialize Cars
	{
		// Create car primitive game object.
		engine::ref<engine::car>       car_shape = engine::car::create(glm::vec3(0.17f), -1);
		engine::game_object_properties car_props;
		car_props.position = {-0.5f, 0.5f, 40.f};
		car_props.rotation_amount = 45;
		car_props.meshes = {car_shape->mesh()};
		car_props.textures = {engine::texture_2d::create("assets/textures/car.png", false)};
		car_props.type = 1;
		car_props.is_static = true;
		car_props.bounding_shape = glm::vec3(0.5f);
		m_car = engine::game_object::create(car_props);

		m_car_data.push_back(car_props.position);
		m_car_data.push_back({car_props.position.x + 1.0f, car_props.position.y + 0.6f, 32.0f});
		m_car_data.push_back({car_props.position.x - 1.0f, car_props.position.y + 0.6f, 28.0f});

		// Car Colliders
		m_car->add_bounding_box(std::make_shared<engine::bounding_box>(3.5f, 0.8f, 2.0f, m_car_data[0]));
		m_car->add_bounding_box(std::make_shared<engine::bounding_box>(0.8f, -0.6f, 1.8f, m_car_data[1]));
		m_car->add_bounding_box(std::make_shared<engine::bounding_box>(0.8f, -0.6f, 1.8f, m_car_data[2]));
	}

	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_car);
	m_game_objects.push_back(m_sign);
	m_game_objects.push_back(m_tree);

	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create("IMFellDWPicaSC-Regular.ttf");

	m_audio_manager = engine::application::audio();
	m_audio_manager->play("level1_ambiance");

	// Being Fade-In transition.
	attach_transition_in_progress = true;
	set_is_layer_active(true);
}

Level1_layer::~Level1_layer() {}

void Level1_layer::attach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = transition_timer / transition_length;

	if (transition_timer <= 0) {
		attach_transition_in_progress = false;

		on_transition_finish();
		on_transition_finish = []()
			{};

		transition_timer = transition_length;
		m_transparency = 0.f;
	}
}

void Level1_layer::detach_scene_transition(const engine::timestep& time_step) {
	transition_timer -= time_step;

	m_transparency = 1.f - transition_timer / transition_length;

	if (transition_timer <= 0) {
		detach_transition_in_progress = false;
		on_transition_finish();
		on_transition_finish = []()
			{};
	}
}

void Level1_layer::on_update(const engine::timestep& time_step) {
	if (attach_transition_in_progress) {
		attach_scene_transition(time_step);
	} else if (detach_transition_in_progress) {
		detach_scene_transition(time_step);
	}

	m_3d_camera.on_update(time_step, *(m_player->bounding_box()[0]));
	m_3d_camera_debug.on_update(time_step);

	// Set player height to height of terrain.
	m_player->set_position(glm::vec3(m_player->position().x, m_heightmap->ground_height(m_player->position()), m_player->position().z));

	const glm::vec3 old_player_pos = m_player->position();

	m_player->on_update(time_step, m_physics_manager);
	m_spotLight.Position = m_player->position() + (0.f * m_player->forward()) + glm::vec3{0.0f, 0.3f, 0.0f};
	m_spotLight.Direction = m_player->forward() + glm::vec3{0.0f, -0.3f, 0.0f};

	// Transition to next level.
	if (m_player->position().z <= 23.0f && !detach_transition_in_progress) {
		detach_transition_in_progress = true;
		transition_length = 3.0f;
		transition_timer = transition_length;

		on_transition_finish = [this]()
		{
			is_layer_active = false;
			engine::application::instance().pop_layer_buffer(this);
			engine::application::instance().push_layer_buffer(new Level2_layer());
		};
	}

	// Check for collisions.
	for (engine::ref<engine::game_object> gm : m_game_objects) {
		for (engine::ref<engine::bounding_box> b_box : gm->bounding_box()) {
			if (b_box->collision(*(m_player->bounding_box()[0])))
				m_player->set_position(old_player_pos);
		}
	}

	sign_texture_timer += time_step;
	if (sign_texture_timer >= 15.f) {
		sign_texture_timer = 0.f;
	}

	// Play player footsteps
	if (m_player->position() != old_player_pos) {
		m_footstep_timer -= time_step;
		if (m_footstep_timer <= 0.0f) {
			m_audio_manager->play_spatialised_sound("snow_footstep" + std::to_string(m_random_footstep_sound(mt)), m_3d_camera.position(), m_player->position());
			m_footstep_timer = m_footstep_timer_cooldown;
		}
	} else {
		m_footstep_timer = m_footstep_timer_cooldown;
	}

	m_audio_manager->update_with_camera(m_3d_camera);
}

void Level1_layer::on_render() {
	engine::render_command::clear_color({0.0f, 0.0f, 0.0f, 1.0f});
	engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(debug_camera ? m_3d_camera_debug : m_3d_camera, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", debug_camera ? m_3d_camera_debug.position() : m_3d_camera.position());
	//m_3d_camera.on_render(mesh_shader);	// Uncomment to show fixed camera angle bounding boxes.

	if (m_show_bounding_boxes) {
		m_material->submit(mesh_shader);
		// Render bounding boxes for all game objects in the level.
		for (engine::ref<engine::game_object> gm : m_game_objects) {
			for (engine::ref<engine::bounding_box> b_box : gm->bounding_box()) {
				b_box->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
			}
		}

		// Render the player's bounding box.
		m_player->bounding_box()[0]->on_render(2.5f, 0.0f, 0.0f, mesh_shader);
	}

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, debug_camera ? m_3d_camera_debug.position() : m_3d_camera.position());
	for (const auto& texture : m_skybox->textures()) {
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(mesh_shader, m_terrain);

	//Temporarily disable culling to render both sides of the sprite/quad. Without the backside of the tree sprite won't be rendered.
	glDisable(GL_CULL_FACE);
	// Enable alpha testing. Discard all fragments with alpha < 0.75f. Without this the tree sprite's alpha component won't work.
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.75f);
	for (int i = 0; i < m_tree_count; i++) {
		m_tree->set_textures({m_tree_textures[std::get<2>(m_trees_data[i])]});

		glm::mat4 tree_transform(1.0f);
		// Use the pre-determined, randomly generated, position for this instance of the tree.
		tree_transform = glm::translate(tree_transform, std::get<0>(m_trees_data[i]));
		tree_transform = glm::rotate(tree_transform, glm::radians(std::get<1>(m_trees_data[i])), m_tree->rotation_axis());
		tree_transform = glm::scale(tree_transform, m_tree->scale());
		engine::renderer::submit(mesh_shader, tree_transform, m_tree);

		// Spawn the same game object but rotated 90 degrees to make the sprite more 3d.
		tree_transform = glm::rotate(tree_transform, glm::radians(90.f), m_tree->rotation_axis());
		engine::renderer::submit(mesh_shader, tree_transform, m_tree);
	}
	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);

	int timer = sign_texture_timer;
	for (size_t i = 0; i < m_signs_data.size(); i++) {
		if (timer % std::get<3>(m_signs_data[i]) == 0) {
			std::get<4>(m_signs_data[i]) = m_dist_sign_texture_index(mt);
		}
		m_sign->set_textures({m_sign_textures[std::get<4>(m_signs_data[i])]});

		glm::mat4 sign_transform(1.0f);
		sign_transform = glm::translate(sign_transform, std::get<0>(m_signs_data[i]));
		sign_transform = glm::rotate(sign_transform, std::get<1>(m_signs_data[i]), m_sign->rotation_axis());
		sign_transform = glm::rotate(sign_transform, std::get<2>(m_signs_data[i]), glm::vec3(0.f, 0.f, 1.f));
		sign_transform = glm::scale(sign_transform, m_sign->scale());
		engine::renderer::submit(mesh_shader, sign_transform, m_sign);
	}

	// Render player
	m_player->on_render(mesh_shader, m_player);
	m_spotLight.submit(mesh_shader, 0);

	// Render car primitive.
	m_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_car);

	// Render second car primitive.
	glm::mat4 car_transform(1.0f);
	car_transform = glm::translate(car_transform, m_car_data[1]);
	car_transform = glm::rotate(car_transform, glm::radians(200.0f), glm::vec3(0.f, 0.f, 1.f));
	car_transform = glm::rotate(car_transform, glm::radians(10.0f), glm::vec3(0.f, 1.f, 0.f));
	car_transform = glm::scale(car_transform, m_car->scale());
	engine::renderer::submit(mesh_shader, car_transform, m_car);

	// Render third car primitive.
	car_transform = glm::mat4(1.0f);
	car_transform = glm::translate(car_transform, m_car_data[2]);
	car_transform = glm::rotate(car_transform, glm::radians(-200.0f), glm::vec3(0.f, 0.f, 1.f));
	car_transform = glm::rotate(car_transform, glm::radians(-190.0f), glm::vec3(0.f, 1.f, 0.f));
	car_transform = glm::scale(car_transform, m_car->scale());
	engine::renderer::submit(mesh_shader, car_transform, m_car);

	// Render mansion image in the background.
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("rho", 0.05f);
	engine::renderer::submit(mesh_shader, m_mansion);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("rho", 0.10f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);

	engine::renderer::end_scene();

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);

	// Render scene transition (Black screen)
	if (attach_transition_in_progress || detach_transition_in_progress) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", m_transparency);
		engine::renderer::submit(mesh_shader, transition_image);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	}

	engine::renderer::end_scene();
}

void Level1_layer::on_event(engine::event& event) {
	m_player->on_event(event, m_physics_manager, m_3d_camera);
	//m_3d_camera.on_event(event, m_3d_camera_debug, *m_player);	// Uncomment to enable fixed camera angle controls.

	if (event.event_type() == engine::event_type_e::key_pressed && !(detach_transition_in_progress || attach_transition_in_progress)) {
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
			std::cout << "Camera Position: " << (debug_camera ? m_3d_camera_debug.position() : m_3d_camera.position()) << std::endl;
		} else if (e.key_code() == engine::key_codes::KEY_R) {
			std::cout << "Player Position: " << m_player->position() << std::endl;
		} else if (e.key_code() == engine::key_codes::KEY_Q) {
			debug_camera = !debug_camera;
		} else if (e.key_code() == engine::key_codes::KEY_T) {
			m_show_bounding_boxes = !m_show_bounding_boxes;
		}
	}
}
