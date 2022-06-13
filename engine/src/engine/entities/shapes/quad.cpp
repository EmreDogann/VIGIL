#include "pch.h"
#include <engine.h>

engine::quad::quad(glm::vec2 half_extents, bool inwards) : m_half_extents(half_extents), m_inwards(inwards)
{
	float orientation = 1;
	if (inwards)
		orientation = -1;

	std::vector<mesh::vertex> quad_vertices
	{
		//front
		//			position														 normal					      tex coord       
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  0 },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 0.f, 0.f } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y,  0 },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 1.f, 0.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y,  0 },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  0 },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 0.f, 1.f } },
	};

	const std::vector<uint32_t> quad_indices
	{
		 0,  1,  2,		 0,  2,  3,  //front
	};

	m_mesh = engine::mesh::create(quad_vertices, quad_indices);
}

engine::quad::~quad() {}

engine::ref<engine::quad> engine::quad::create(glm::vec2 half_extents, bool inwards)
{
	return std::make_shared<engine::quad>(half_extents, inwards);
}
