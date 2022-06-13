#include "pch.h"
#include "character.h"
#include "engine/entities/game_object.h"

engine::character::character(ref<game_object> object) : m_object(object) {}

engine::character::~character() {}

void engine::character::move(glm::vec3 accelerate, double dt)
{
	m_object->set_acceleration(accelerate);
}

void engine::character::turn(glm::vec3 torque, double dt)
{
	m_object->set_torque(torque);
}
