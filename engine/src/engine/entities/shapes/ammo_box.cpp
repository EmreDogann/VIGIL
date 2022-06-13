#include "pch.h"
#include <engine.h>

engine::ammo_box::ammo_box(glm::vec3 half_extents, bool inwards) : m_half_extents(half_extents), m_inwards(inwards) {
	float orientation = 1;
	if (inwards)
		orientation = -1;

	// Apologies for the formatting. Resharper formatting just doesn't want to cooperate. :/
	std::vector<mesh::vertex> ammo_box_vertices{
		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.0f, 0.69f}
		},
		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.28f, 0.14f}
		},
		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.28f, 0.69f}
		},

		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.28f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.0f, 0.0f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.28f, 0.0f}
		},

		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.82f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.69f, 0.69f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.69f, 0.14f}
		},

		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.41f, 0.69f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.69f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.69f, 0.69f}
		},

		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.28f, 0.69f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.41f, 0.14f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.41f, 0.69f}
		},

		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, 1.0f * orientation}, {0.69f, 0.82f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, 1.0f * orientation}, {0.41f, 0.69f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, 1.0f * orientation}, {0.69f, 0.69f}
		},

		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.94f, 0.42f}
		},
		{
			{-2.0f * m_half_extents.x, 1.58f * m_half_extents.y, -5.73f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.83f, 0.53f}
		},
		{
			{-2.0f * m_half_extents.x, 0.42f * m_half_extents.y, -5.73f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.83f, 0.45f}
		},

		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.94f, 0.41f}
		},
		{
			{2.0f * m_half_extents.x, 1.58f * m_half_extents.y, -5.73f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.83f, 0.31f}
		},
		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.94f, 0.28f}
		},

		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {1.0f, 0.28f}
		},
		{
			{1.64f * m_half_extents.x, 0.0f * m_half_extents.y, -5.99f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.86f, 0.03f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {1.0f, 0.0f}
		},

		{
			{-1.64f * m_half_extents.x, 0.0f * m_half_extents.y, -5.99f * m_half_extents.z},
			{0.0f * orientation, -0.9f * orientation, -0.43f * orientation}, {0.86f, 0.25f}
		},
		{
			{1.44f * m_half_extents.x, 0.23f * m_half_extents.y, -6.48f * m_half_extents.z},
			{0.0f * orientation, -0.9f * orientation, -0.43f * orientation}, {0.83f, 0.04f}
		},
		{
			{1.64f * m_half_extents.x, 0.0f * m_half_extents.y, -5.99f * m_half_extents.z},
			{0.0f * orientation, -0.9f * orientation, -0.43f * orientation}, {0.86f, 0.03f}
		},

		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.0f, 0.69f}
		},
		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.0f, 0.14f}
		},
		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.28f, 0.14f}
		},

		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.28f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.0f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.0f, 0.0f}
		},

		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.82f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.82f, 0.69f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.69f, 0.69f}
		},

		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.41f, 0.69f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.41f, 0.14f}
		},
		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.69f, 0.14f}
		},

		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.28f, 0.69f}
		},
		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.28f, 0.14f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.41f, 0.14f}
		},

		{
			{2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, 1.0f * orientation}, {0.69f, 0.82f}
		},
		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, 1.0f * orientation}, {0.41f, 0.82f}
		},
		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, 4.0f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, 1.0f * orientation}, {0.41f, 0.69f}
		},

		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.94f, 0.42f}
		},
		{
			{-2.0f * m_half_extents.x, 2.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.94f, 0.55f}
		},
		{
			{-2.0f * m_half_extents.x, 1.58f * m_half_extents.y, -5.73f * m_half_extents.z},
			{-1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.83f, 0.53f}
		},

		{
			{2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.94f, 0.41f}
		},
		{
			{2.0f * m_half_extents.x, 0.42f * m_half_extents.y, -5.73f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.83f, 0.39f}
		},
		{
			{2.0f * m_half_extents.x, 1.58f * m_half_extents.y, -5.73f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.83f, 0.31f}
		},

		{
			{-2.0f * m_half_extents.x, 0.0f * m_half_extents.y, -4.0f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {1.0f, 0.28f}
		},
		{
			{-1.64f * m_half_extents.x, 0.0f * m_half_extents.y, -5.99f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.86f, 0.25f}
		},
		{
			{1.64f * m_half_extents.x, 0.0f * m_half_extents.y, -5.99f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.86f, 0.03f}
		},

		{
			{-1.64f * m_half_extents.x, 0.0f * m_half_extents.y, -5.99f * m_half_extents.z},
			{0.0f * orientation, -0.9f * orientation, -0.43f * orientation}, {0.86f, 0.25f}
		},
		{
			{-1.44f * m_half_extents.x, 0.23f * m_half_extents.y, -6.48f * m_half_extents.z},
			{0.0f * orientation, -0.9f * orientation, -0.43f * orientation}, {0.83f, 0.24f}
		},
		{
			{1.44f * m_half_extents.x, 0.23f * m_half_extents.y, -6.48f * m_half_extents.z},
			{0.0f * orientation, -0.9f * orientation, -0.43f * orientation}, {0.83f, 0.04f}
		},

	};
	const std::vector<uint32_t> ammo_box_spray_indices{

		0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35,
		36, 37, 38, 39, 40, 41,
		42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53,
		54, 55, 56, 57, 58, 59,
	};

	m_mesh = engine::mesh::create(ammo_box_vertices, ammo_box_spray_indices);
}

engine::ammo_box::~ammo_box() {}

engine::ref<engine::ammo_box> engine::ammo_box::create(glm::vec3 half_extents, bool inwards) {
	return std::make_shared<engine::ammo_box>(half_extents, inwards);
}
