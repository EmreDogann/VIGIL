#pragma once
#include "engine/core.h"

namespace engine {
	class game_object;
}

class Pickup {
public:
	virtual void useItem() = 0;
	virtual void discardItem() = 0;

	virtual bool at_max_quantity() {
		return m_quantity == m_max_quantity;
	}

	virtual int get_quantity() {
		return m_quantity;
	}

	virtual int get_max_quantity() {
		return m_quantity;
	}

	virtual void set_quantity(int new_quantity) {
		m_quantity = new_quantity;
	}

	int get_inventory_position() {
		return m_inventory_slot_position;
	}

	std::string get_description() {
		return m_description;
	}

	virtual std::vector<engine::ref<engine::game_object>> get_associated_objects() {
		std::vector<engine::ref<engine::game_object>> empty;
		return empty;
	}

	void set_inventory_position(int new_pos) {
		m_inventory_slot_position = new_pos;
	}

	engine::ref<engine::game_object> get_object() {
		return m_object;
	}

	bool is_equipped() {
		return m_equipped;
	}

	void set_equipped(bool val) {
		m_equipped = val;
	}

protected:
	int                              m_quantity;
	int                              m_max_quantity;
	int                              m_inventory_slot_position;
	std::string                      m_description;
	engine::ref<engine::game_object> m_object;
	bool                             m_equipped = false;
};
