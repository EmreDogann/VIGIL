#include "FirstAidSpray.h"

FirstAidSpray::FirstAidSpray() {
	m_max_quantity = 1;
	m_quantity = m_max_quantity;
	m_description = "First Aid Spray";

	// Create First Aid Spray primitive game object.
	engine::ref<engine::first_aid_spray> first_aid_spray_shape = engine::first_aid_spray::create(glm::vec3(0.035f), -1);
	engine::game_object_properties       first_aid_spray_props;
	first_aid_spray_props.position = {0.0f, -0.09f, 0.0f};
	first_aid_spray_props.rotation_axis = glm::vec3(0.5f, 1.0f, 0.0f);
	first_aid_spray_props.rotation_amount = glm::radians(45.0f);
	first_aid_spray_props.meshes = {first_aid_spray_shape->mesh()};
	first_aid_spray_props.textures = {engine::texture_2d::create("assets/textures/FirstAidSpray.png", false)};
	first_aid_spray_props.type = 1;
	first_aid_spray_props.tag = "Pickup";
	first_aid_spray_props.is_static = true;
	first_aid_spray_props.bounding_shape = glm::vec3(0.5f);
	m_object = engine::game_object::create(first_aid_spray_props);

	// First Aid Spray Collider
	m_object->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 0.1f, 0.3f, m_object->position()));
}

void FirstAidSpray::useItem() {}
void FirstAidSpray::discardItem() {}
