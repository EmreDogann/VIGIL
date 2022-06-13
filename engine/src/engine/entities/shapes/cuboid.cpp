#include "pch.h"
#include "cuboid.h"
#include <engine.h>

engine::cuboid::cuboid(glm::vec3 half_extents, bool inwards) : m_half_extents(half_extents), m_inwards(inwards) {
	float orientation = 1;
	if (inwards)
		orientation = -1;

	std::vector<mesh::vertex> cuboid_vertices
	{
		//front
		//                                 position														 normal					      tex coord       
		{{-1.f * m_half_extents.x, -1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, 0.0f, 1.0f * orientation}, {0.f, 0.f}},
		{{1.f * m_half_extents.x, -1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, 0.0f, 1.0f * orientation}, {1.f, 0.f}},
		{{1.f * m_half_extents.x, 1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, 0.0f, 1.0f * orientation}, {1.f, 1.f}},
		{{-1.f * m_half_extents.x, 1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, 0.0f, 1.0f * orientation}, {0.f, 1.f}},

		//right
		{{1.f * m_half_extents.x, -1.f * m_half_extents.y, 1.f * m_half_extents.z}, {1.0f * orientation, 0.0f, 0.0f}, {0.f, 0.f}},
		{{1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z}, {1.0f * orientation, 0.0f, 0.0f}, {1.f, 0.f}},
		{{1.f * m_half_extents.x, 1.f * m_half_extents.y, -1.f * m_half_extents.z}, {1.0f * orientation, 0.0f, 0.0f}, {1.f, 1.f}},
		{{1.f * m_half_extents.x, 1.f * m_half_extents.y, 1.f * m_half_extents.z}, {1.0f * orientation, 0.0f, 0.0f}, {0.f, 1.f}},

		//back
		{{1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, 0.0f, -1.0f * orientation}, {0.f, 0.f}},
		{{-1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, 0.0f, -1.0f * orientation}, {1.f, 0.f}},
		{{-1.f * m_half_extents.x, 1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, 0.0f, -1.0f * orientation}, {1.f, 1.f}},
		{{1.f * m_half_extents.x, 1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, 0.0f, -1.0f * orientation}, {0.f, 1.f}},

		//left
		{{-1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z}, {-1.0f * orientation, 0.0f, 0.0f}, {0.f, 0.f}},
		{{-1.f * m_half_extents.x, -1.f * m_half_extents.y, 1.f * m_half_extents.z}, {-1.0f * orientation, 0.0f, 0.0f}, {1.f, 0.f}},
		{{-1.f * m_half_extents.x, 1.f * m_half_extents.y, 1.f * m_half_extents.z}, {-1.0f * orientation, 0.0f, 0.0f}, {1.f, 1.f}},
		{{-1.f * m_half_extents.x, 1.f * m_half_extents.y, -1.f * m_half_extents.z}, {-1.0f * orientation, 0.0f, 0.0f}, {0.f, 1.f}},

		//top
		{{-1.f * m_half_extents.x, 1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, 1.0f * orientation, 0.0f}, {0.f, 0.f}},
		{{1.f * m_half_extents.x, 1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, 1.0f * orientation, 0.0f}, {100.f, 0.f}},
		{{1.f * m_half_extents.x, 1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, 1.0f * orientation, 0.0f}, {100.f, 100.f}},
		{{-1.f * m_half_extents.x, 1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, 1.0f * orientation, 0.0f}, {0.f, 100.f}},

		//bottom
		{{-1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, -1.0f * orientation, 0.0f}, {0.f, 0.f}},
		{{1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z}, {0.0f, -1.0f * orientation, 0.0f}, {1.f, 0.f}},
		{{1.f * m_half_extents.x, -1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, -1.0f * orientation, 0.0f}, {1.f, 1.f}},
		{{-1.f * m_half_extents.x, -1.f * m_half_extents.y, 1.f * m_half_extents.z}, {0.0f, -1.0f * orientation, 0.0f}, {0.f, 1.f}},
	};

	const std::vector<uint32_t> cuboid_indices
	{
		0, 1, 2, 0, 2, 3,       //front
		4, 5, 6, 4, 6, 7,       //right
		8, 9, 10, 8, 10, 11,    //back
		12, 13, 14, 12, 14, 15, //left
		16, 17, 18, 16, 18, 19, //upper
		20, 21, 22, 20, 22, 23  //bottom
	};

	m_mesh = engine::mesh::create(cuboid_vertices, cuboid_indices);
}

engine::cuboid::~cuboid() {}

engine::ref<engine::cuboid> engine::cuboid::create(glm::vec3 half_extents, bool inwards) {
	return std::make_shared<engine::cuboid>(half_extents, inwards);
}
