#include "Pistol.h"

Pistol::Pistol() {
	m_max_quantity = 1;
	m_quantity = m_max_quantity;
	m_description = "Beretta 9mm Handgun";

	// Load the gun model. Create a gun object. Set its properties.
	engine::ref<engine::model>     gun_pickup = engine::model::create("assets/models/static/beretta/gun.fbx");
	engine::game_object_properties gun_pickup_props;
	gun_pickup_props.meshes = gun_pickup->meshes();
	gun_pickup_props.textures = {engine::texture_2d::create("assets/models/static/beretta/M9_palette.png", false)};
	gun_pickup_props.type = 0;
	gun_pickup_props.tag = "Pickup";

	float gun_scale = 1.f / glm::max(gun_pickup->size().x, glm::max(gun_pickup->size().y, gun_pickup->size().z));
	gun_pickup_props.position = {0.0f, 0.0f, 0.0f};
	gun_pickup_props.rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
	gun_pickup_props.rotation_amount = glm::radians(90.0f);
	gun_pickup_props.scale = glm::vec3(gun_scale) / 5.0f;
	gun_pickup_props.bounding_shape = gun_pickup->size() / 2.f * gun_scale;
	gun_pickup_props.is_static = true;
	m_object = engine::game_object::create(gun_pickup_props);

	m_object->add_bounding_box(std::make_shared<engine::bounding_box>(0.5f, 0.1f, 0.5f, m_object->position()));

	// Load the bullet model. Create a bullet object. Set its properties.
	engine::ref<engine::model>     bullet_model = engine::model::create("assets/models/static/bullet.fbx");
	engine::game_object_properties bullet_props;
	bullet_props.meshes = bullet_model->meshes();
	bullet_props.type = 0;
	bullet_props.tag = "Bullet";

	float bullet_scale = 1.f / glm::max(bullet_model->size().x, glm::max(bullet_model->size().y, bullet_model->size().z));
	bullet_props.position = {0.0f, 0.0f, 0.0f};
	bullet_props.scale = glm::vec3(bullet_scale) / 15.0f;
	bullet_props.scale.z /= 1.5f;
	bullet_props.mass = 0.001f;
	bullet_props.friction = 0.01f;
	bullet_props.bounding_shape = bullet_model->size() / 2.f * bullet_scale;

	for (int i = 0; i < m_capacity; i++) {
		m_bullet_objects.push_back(engine::game_object::create(bullet_props));
		m_bullet_objects[i]->set_angular_factor_lock(true);
		m_bullet_objects[i]->add_bounding_box(std::make_shared<engine::bounding_box>(0.1f, 0.1f, 0.1f, m_bullet_objects[i]->position()));
	}
}

void Pistol::useItem() {
	m_bullets_remaining--;
}

void Pistol::discardItem() {}
