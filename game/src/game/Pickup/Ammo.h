#pragma once
#include "Pickup.h"
#include <engine.h>

class Ammo : public Pickup {
public:
	Ammo();
	~Ammo() = default;

	void useItem() override;
	void discardItem() override;

	int get_quantity() override {
		return m_bullets_remaining;
	}

	void set_quantity(int new_quantity) override {
		m_bullets_remaining = new_quantity;
	}

	int get_max_quantity() override {
		return m_max_bullets;
	}

private:
	int m_bullets_remaining = 15;
	int m_max_bullets = 15;
};
