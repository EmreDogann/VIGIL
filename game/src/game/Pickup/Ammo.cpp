#include "Ammo.h"

Ammo::Ammo() {
	m_max_quantity = 15;
	m_quantity = m_max_quantity;
	m_description = "9mm Handgun Ammo";

	// Create Ammo Box primitive game object.
	engine::ref<engine::ammo_box>  ammo_box_shape = engine::ammo_box::create(glm::vec3(0.020f), -1);
	engine::game_object_properties ammo_box_props;
	ammo_box_props.position = {0.025f, 0.0f, 0.0f};
	ammo_box_props.rotation_axis = glm::vec3(1.0f, 0.0f, 1.0f);
	ammo_box_props.rotation_amount = glm::radians(90.0f);
	ammo_box_props.meshes = {ammo_box_shape->mesh()};
	ammo_box_props.textures = {engine::texture_2d::create("assets/textures/AmmoBox.png", false)};
	ammo_box_props.type = 2;
	ammo_box_props.tag = "Pickup";
	ammo_box_props.is_static = true;
	ammo_box_props.bounding_shape = glm::vec3(0.5f);
	m_object = engine::game_object::create(ammo_box_props);

	// Ammo Box Collider
	m_object->add_bounding_box(std::make_shared<engine::bounding_box>(0.3f, 0.1f, 0.3f, m_object->position()));
}

void Ammo::useItem() {}
void Ammo::discardItem() {}
