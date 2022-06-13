#pragma once
#include <engine.h>

class BulletProjectile : public engine::game_object {
public:
	BulletProjectile(const engine::game_object_properties& props);
	~BulletProjectile();

	static engine::ref<BulletProjectile> create();

	void on_update(const engine::timestep& time_step);

	void on_render(const engine::ref<engine::shader>& shader);

	void set_damage(float damage) {
		m_damage = damage;
	}

private:
	float m_damage = 20.0f;
};
