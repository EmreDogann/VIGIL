#include "pch.h"
#include <engine.h>

engine::first_aid_spray::first_aid_spray(glm::vec3 half_extents, bool inwards) : m_half_extents(half_extents), m_inwards(inwards) {
	float orientation = 1;
	if (inwards)
		orientation = -1;

	// Apologies for the formatting. Resharper formatting just doesn't want to cooperate. :/
	std::vector<mesh::vertex> first_aid_spray_vertices{
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.0f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.22f, 0.0f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.22f, 0.78f}
		},

		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.66f, 0.78f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.88f, -0.0f}
		},
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.88f, 0.78f}
		},

		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, -0.69f * orientation}, {0.08f, 0.86f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, -0.69f * orientation}, {0.22f, 0.78f}
		},
		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, -0.69f * orientation}, {0.14f, 0.86f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, 0.69f * orientation}, {0.14f, 0.92f}
		},
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, 0.69f * orientation}, {0.0f, 1.0f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, 0.69f * orientation}, {0.08f, 0.92f}
		},

		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.61f, 0.85f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.55f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.61f, 0.78f}
		},

		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.69f * orientation, 0.73f * orientation, 0.0f * orientation}, {0.0f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.69f * orientation, 0.73f * orientation, 0.0f * orientation}, {0.08f, 0.92f}
		},
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.69f * orientation, 0.73f * orientation, 0.0f * orientation}, {0.0f, 1.0f}
		},

		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.44f, 0.78f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.22f, 1.0f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.22f, 0.78f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.69f * orientation, 0.73f * orientation, -0.0f * orientation}, {0.14f, 0.92f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{-0.69f * orientation, 0.73f * orientation, -0.0f * orientation}, {0.22f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.69f * orientation, 0.73f * orientation, -0.0f * orientation}, {0.22f, 1.0f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.55f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.5f, 0.85f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.5f, 0.78f}
		},

		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.72f, 0.83f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.66f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.72f, 0.78f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.44f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.5f, 0.85f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.44f, 0.85f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.66f, 0.78f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.61f, 0.85f}
		},
		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.61f, 0.78f}
		},

		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.22f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.44f, 0.0f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.44f, 0.78f}
		},

		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.44f, 0.78f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.66f, 0.0f}
		},
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.66f, 0.78f}
		},

		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.0f, 0.78f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.0f, 0.0f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.22f, 0.0f}
		},

		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.66f, 0.78f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.66f, 0.0f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.88f, -0.0f}
		},

		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, -0.69f * orientation}, {0.08f, 0.86f}
		},
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, -0.69f * orientation}, {0.0f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, -0.69f * orientation}, {0.22f, 0.78f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, 0.69f * orientation}, {0.14f, 0.92f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, 0.69f * orientation}, {0.22f, 1.0f}
		},
		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.0f * orientation, 0.73f * orientation, 0.69f * orientation}, {0.0f, 1.0f}
		},

		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.61f, 0.85f}
		},
		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.55f, 0.85f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{1.0f * orientation, 0.0f * orientation, 0.0f * orientation}, {0.55f, 0.78f}
		},

		{
			{0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.69f * orientation, 0.73f * orientation, 0.0f * orientation}, {0.0f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.69f * orientation, 0.73f * orientation, 0.0f * orientation}, {0.08f, 0.86f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.69f * orientation, 0.73f * orientation, 0.0f * orientation}, {0.08f, 0.92f}
		},

		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.44f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.44f, 1.0f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{0.0f * orientation, -1.0f * orientation, 0.0f * orientation}, {0.22f, 1.0f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.69f * orientation, 0.73f * orientation, -0.0f * orientation}, {0.14f, 0.92f}
		},
		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{-0.69f * orientation, 0.73f * orientation, -0.0f * orientation}, {0.14f, 0.86f}
		},
		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{-0.69f * orientation, 0.73f * orientation, -0.0f * orientation}, {0.22f, 0.78f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.55f, 0.78f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.55f, 0.85f}
		},
		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 0.0f * orientation, -1.0f * orientation}, {0.5f, 0.85f}
		},

		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.72f, 0.83f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.66f, 0.83f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{0.0f * orientation, 1.0f * orientation, 0.0f * orientation}, {0.66f, 0.78f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.44f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.5f, 0.78f}
		},
		{
			{0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.5f, 0.85f}
		},

		{
			{-0.18f * m_half_extents.x, 5.5f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.66f, 0.78f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, 0.18f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.66f, 0.85f}
		},
		{
			{-0.18f * m_half_extents.x, 5.98f * m_half_extents.y, -0.18f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.61f, 0.85f}
		},

		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.22f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, -0.71f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.22f, 0.0f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-1.0f * orientation, -0.0f * orientation, -0.0f * orientation}, {0.44f, 0.0f}
		},

		{
			{-0.71f * m_half_extents.x, 5.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.44f, 0.78f}
		},
		{
			{-0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.44f, 0.0f}
		},
		{
			{0.71f * m_half_extents.x, 0.0f * m_half_extents.y, 0.71f * m_half_extents.z},
			{-0.0f * orientation, -0.0f * orientation, 1.0f * orientation}, {0.66f, 0.0f}
		},

	};
	const std::vector<uint32_t> first_aid_spray_indices{

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
		60, 61, 62, 63, 64, 65,
		66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77,
		78, 79, 80, 81, 82, 83,
	};

	m_mesh = engine::mesh::create(first_aid_spray_vertices, first_aid_spray_indices);
}

engine::first_aid_spray::~first_aid_spray() {}

engine::ref<engine::first_aid_spray> engine::first_aid_spray::create(glm::vec3 half_extents, bool inwards) {
	return std::make_shared<engine::first_aid_spray>(half_extents, inwards);
}
