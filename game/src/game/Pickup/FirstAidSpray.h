#pragma once
#include "Pickup.h"
#include <engine.h>

class FirstAidSpray : public Pickup {
public:
	FirstAidSpray();
	~FirstAidSpray() = default;

	void useItem() override;
	void discardItem() override;

	int get_quantity() override {
		return m_heal_amount;
	}

private:
	int m_heal_amount = 50;
};
