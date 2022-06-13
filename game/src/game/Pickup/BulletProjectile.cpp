#include "BulletProjectile.h"
BulletProjectile::BulletProjectile(const engine::game_object_properties& props) : game_object(props) {}

BulletProjectile::~BulletProjectile() {}

engine::ref<BulletProjectile> BulletProjectile::create() {
	engine::ref<engine::model> bullet_model = engine::model::create("assets/models/animated/player/model-gun.fbx");

	engine::game_object_properties bullet_props;
	bullet_props.meshes = bullet_model->meshes();
	bullet_props.scale = glm::vec3(.45f);
	bullet_props.type = 0;
	bullet_props.bounding_shape = glm::vec3(0.2f, 0.1f, 0.2f);
	return std::make_shared<BulletProjectile>(bullet_props);
}

void BulletProjectile::on_update(const engine::timestep& time_step) {}

void BulletProjectile::on_render(const engine::ref<engine::shader>& shader) {}
