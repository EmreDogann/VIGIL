#include "pch.h"
#include "material.h"
#include "platform/opengl/gl_shader.h"

engine::material::material(const float shininess,
	const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, float transparency)
	: m_shininess(shininess), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_transparency(transparency)
{
}

engine::material::~material()
{
}

engine::ref<engine::material> engine::material::create(const float shininess,
	const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, float transparency)
{
	return std::make_shared<engine::material>(engine::material(shininess, ambient, diffuse, specular, transparency));
}

void engine::material::submit(const engine::ref<engine::shader> shader)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("material.shininess", m_shininess);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("material.ambient", m_ambient);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("material.diffuse", m_diffuse);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("material.specular", m_specular);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("material.transparency", m_transparency);
}
