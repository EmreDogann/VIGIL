#include "pch.h"
#include "sphere.h"
#include <engine.h>

engine::sphere::sphere(uint32_t stacks, uint32_t slices, float radius) : m_stacks(stacks), m_slices(slices), m_radius(radius)
{
	std::vector<engine::mesh::vertex> sphere_vertices;
	std::vector<uint32_t> sphere_indices;

	const float sliceStep = 2 * PI / m_slices;
	const float stackStep = PI / m_stacks;
	const float lengthInv = 1.0f / m_radius;

	for (uint32_t i = 0; i <= m_stacks; ++i)
	{
		const float stackAngle = PI / 2 - i * stackStep;  // starting from pi/2 to -pi/2
		float xy = m_radius * cosf(stackAngle);             // r * cos(u)
		float z = m_radius * sinf(stackAngle);              // r * sin(u)

		// add (sliceCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (uint32_t j = 0; j <= m_slices; ++j)
		{
			const float sliceAngle = j * sliceStep;       // starting from 0 to 2pi

			// vertex position (x, y, z)
			const float pos_x = xy * cosf(sliceAngle);    // r * cos(u) * cos(v)
			const float pos_y = xy * sinf(sliceAngle);    // r * cos(u) * sin(v)
			const float pos_z = z;

			// normalized vertex normal (x, y, z)
			float norm_x = pos_x * lengthInv;
			float norm_y = pos_y * lengthInv;
			float norm_z = pos_z * lengthInv;

			// vertex tex coord (x, y) range between [0, 1]
			float tex_y = 1.0f - static_cast<float>(j) / m_slices;
			float tex_x = static_cast<float>(i) / m_stacks;

			mesh::vertex vertex(glm::vec3(pos_x, pos_y, pos_z),
				glm::vec3(norm_x, norm_y, norm_z), glm::vec2(tex_x, tex_y));

			sphere_vertices.push_back(vertex);
		}
	}

	for (uint32_t i = 0; i < m_stacks; ++i)
	{
		uint32_t k1 = i * (m_slices + 1);	  // beginning of current stack
		uint32_t k2 = k1 + m_slices + 1;      // beginning of next stack

		for (uint32_t j = 0; j < m_slices; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				sphere_indices.push_back(k1);
				sphere_indices.push_back(k2);
				sphere_indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (m_stacks - 1))
			{
				sphere_indices.push_back(k1 + 1);
				sphere_indices.push_back(k2);
				sphere_indices.push_back(k2 + 1);
			}
		}
	}

	m_mesh = engine::mesh::create(sphere_vertices, sphere_indices);
}

engine::sphere::~sphere() {}

engine::ref<engine::sphere> engine::sphere::create(uint32_t stacks, uint32_t slices, float radius)
{
	return std::make_shared<engine::sphere>(stacks, slices, radius);
}
