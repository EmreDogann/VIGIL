#include "pch.h"
#include "game_object.h"

#include "bounding_box.h"
#include "engine/renderer/texture.h"

// TODO - see header ;)
engine::game_object::game_object(const game_object_properties& props) : m_position(props.position),
                                                                        m_velocity(props.velocity),
                                                                        m_rotation_axis(props.rotation_axis),
                                                                        m_rotation_amount(props.rotation_amount),
                                                                        m_meshes(props.meshes),
                                                                        m_textures(props.textures),
                                                                        m_animated_mesh(props.animated_mesh),
                                                                        m_scale(props.scale),
                                                                        m_mass(props.mass),
                                                                        m_restitution(props.restitution),
                                                                        m_friction(props.friction),
                                                                        s_static(props.is_static),
                                                                        m_type(props.type),
                                                                        m_tag(props.tag),
                                                                        m_bounding_shape(props.bounding_shape) {}

/*engine::game_object::game_object(const glm::vec3 position, const glm::vec3 velocity, std::vector<ref<mesh>> meshes,
    const glm::vec3 scale, const bool is_static, float mass, int32_t type, glm::vec3 bounding_shape,
    glm::vec3 rotation_axis, float rotation_amount)
    : m_position(position), m_velocity(velocity), m_scale(scale), m_static(is_static), m_meshes(meshes), m_mass(mass),
    m_type(type), m_bounding_shape(bounding_shape), m_rotation_axis(rotation_axis), m_rotation_amount(rotation_amount)
{
}

engine::game_object::game_object(std::vector<ref<mesh>> meshes, int32_t type, glm::vec3 bounding_shape)
    : m_meshes(meshes), m_type(type), m_bounding_shape(bounding_shape)
{
}

engine::game_object::game_object(const bool is_static, int32_t type, glm::vec3 bounding_shape)
    : m_static(is_static), m_type(type), m_bounding_shape(bounding_shape)
{
}*/

engine::game_object::~game_object() {}

const void engine::game_object::transform(glm::mat4& object_transform) {
	// apply object's transformations including an offset to synchronise it with the bullet_physics simulation
	// by centering the object's centre of mass on its position
	object_transform = glm::translate(object_transform, position() - offset() * scale());
	object_transform = glm::rotate(object_transform, rotation_amount(), rotation_axis());
	object_transform = glm::scale(object_transform, scale());
}

void engine::game_object::bind_textures() {
	// no need to check if texture is empty since the for loop will not run if that's the case
	auto object_textures = m_textures;
	for (const auto& texture : object_textures) {
		texture->bind();
	}
}

void engine::game_object::turn_towards(glm::vec3 direction) {
	glm::vec3 norm_direction = glm::normalize(direction);
	set_rotation_axis(glm::cross(norm_direction, glm::vec3(0.0f, 0.0f, 1.0f)));
	float rotation = -acos(glm::dot(norm_direction, glm::vec3(0.0f, 0.0f, 1.0f)));
	set_rotation_amount(rotation);
}

// TODO - use this once you fix the above [and back (header)]
engine::ref<engine::game_object> engine::game_object::create(const engine::game_object_properties& props) {
	return std::make_shared<engine::game_object>(props);
}
