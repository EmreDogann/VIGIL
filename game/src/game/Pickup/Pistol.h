#pragma once
#include "Pickup.h"
#include <engine.h>

class Pistol : public Pickup {
public:
	Pistol();
	~Pistol() = default;

	void useItem() override;
	void discardItem() override;

	int get_quantity() override {
		return m_bullets_remaining;
	}

	void set_quantity(int new_quantity) override {
		m_bullets_remaining = new_quantity;
	}

	bool at_max_quantity() override {
		return m_bullets_remaining == m_capacity;
	}

	int get_max_quantity() override {
		return m_capacity;
	}

	std::vector<engine::ref<engine::game_object>> get_associated_objects() override {
		return m_bullet_objects;
	}

private:
	int                                           m_bullets_remaining = 15;
	int                                           m_capacity = 15;
	std::vector<engine::ref<engine::game_object>> m_bullet_objects;
};
