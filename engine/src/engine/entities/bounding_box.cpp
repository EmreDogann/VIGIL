#include "pch.h"
#include "bounding_box.h"
#include "platform/opengl/gl_shader.h"

engine::bounding_box::bounding_box(float width, float height, float depth, glm::vec3 position) {
	set_box(width, height, depth, position);
}

engine::bounding_box::~bounding_box() {}

void engine::bounding_box::get(glm::vec3& bottom, float& width, float& height, float& depth) {
	bottom = m_bottom;
	width = m_width;
	height = m_height;
	depth = m_depth;
}

void engine::bounding_box::set_box(float width, float height, float depth, glm::vec3 position) {
	m_width = width;
	m_height = height;
	m_depth = depth;

	m_bottom = position - glm::vec3(m_width / 2.f, 0.0f, m_depth / 2.f);

	std::vector<engine::mesh::vertex> grid_vertices
	{
		{glm::vec3(0.f, 0.f, 0.f), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(width, 0.f, 0.f), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(width, 0.f, depth), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(0.f, 0.f, depth), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(0.f, height, 0.f), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(width, height, 0.f), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(width, height, depth), {0.f, 1.f, 0.f}, {0.f, 0.f}},
		{glm::vec3(0.f, height, depth), {0.f, 1.f, 0.f}, {0.f, 0.f}},
	};
	std::vector<uint32_t> grid_indices
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		4, 5,
		5, 6,
		6, 7,
		7, 4,

		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};


	m_mesh = engine::mesh::create(grid_vertices, grid_indices);
}

void engine::bounding_box::on_update(glm::vec3 position) {
	m_bottom = position - glm::vec3(m_width / 2.f, 0.f, m_depth / 2.f);
}

void engine::bounding_box::on_render(float r, float g, float b, const engine::ref<engine::shader>& shader) {
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_bottom);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("colouring_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("in_colour", glm::vec3(r, g, b));
	m_mesh->va()->bind();
	engine::renderer_api::draw_indexed_lines(m_mesh->va());
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("colouring_on", false);
}


bool engine::bounding_box::collision(bounding_box other_box) {
	float     width1,  height1, depth1,  width2, height2, depth2;
	glm::vec3 bottom1, bottom2, centre1, centre2;

	get(bottom1, width1, height1, depth1);           // this box
	other_box.get(bottom2, width2, height2, depth2); // other box
	centre1 = bottom1 + glm::vec3(width1 / 2.f, height1 / 2.0f, depth1 / 2.f);
	centre2 = bottom2 + glm::vec3(width2 / 2.f, height2 / 2.0f, depth2 / 2.f);

	glm::vec3 v = centre2 - centre1;
	if (fabs(v.x) * 2 <= (width1 + width2) &&
		fabs(v.y) * 2 <= (height1 + height2) &&
		fabs(v.z) * 2 <= (depth1 + depth2))
		return true; // Collision detected
	else
		return false; // No collision detected
}
