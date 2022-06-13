#include "pch.h"
#include "grid.h"
#include "platform/opengl/gl_shader.h"

engine::grid::grid(uint32_t length, uint32_t width, uint32_t step) : m_length(length), m_width(width), m_step(step)
{
	std::vector<mesh::vertex> grid_vertices;
	std::vector<uint32_t> grid_indices;
	uint32_t vertex_count = 0;
	for (int i = -m_width; i < m_width; i += m_step)
	{
		mesh::vertex v0(glm::vec3(-m_width, 0.f, i),
			glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, i + m_length/(2.f* m_length)));
		grid_vertices.push_back(v0);
		grid_indices.push_back(vertex_count);
		vertex_count++;
		mesh::vertex v1(glm::vec3(m_width, 0.f, i),
			glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, i + m_length / (2.f * m_length)));
		grid_vertices.push_back(v1);
		//grid_indices.push_back(vertex_count + 1);
		grid_indices.push_back(vertex_count);
		
		vertex_count++;
	}

	for (int i = -m_length; i < m_length; i += m_step)
	{
		mesh::vertex v0(glm::vec3(i, 0.f, -m_length),
			glm::vec3(0.f, 1.f, 0.f), glm::vec2(i + m_width / (2.f * m_width), 0.f));
		grid_vertices.push_back(v0);
		grid_indices.push_back(vertex_count);
		vertex_count++;
		mesh::vertex v1(glm::vec3(i, 0.f, m_length),
			glm::vec3(0.f, 1.f, 0.f), glm::vec2(i + m_width / (2.f * m_width), 1.0f));
		grid_vertices.push_back(v1);
		grid_indices.push_back(vertex_count);
		//grid_indices.push_back(vertex_count+1);
		vertex_count++;
	}

	m_mesh = engine::mesh::create(grid_vertices, grid_indices);
}

engine::grid::~grid() {}

engine::ref<engine::grid> engine::grid::create(uint32_t length, uint32_t width, uint32_t step)
{
	return std::make_shared<engine::grid>(length, width, step);
}

void engine::grid::on_render(const ref<shader>& shader)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", glm::mat4(1.0f));
	m_mesh->va()->bind();
	engine::renderer_api::draw_indexed_lines(m_mesh->va());
}
