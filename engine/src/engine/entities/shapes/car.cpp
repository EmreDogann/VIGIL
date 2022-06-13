#include "pch.h"
#include <engine.h>

engine::car::car(glm::vec3 half_extents, bool inwards) : m_half_extents(half_extents), m_inwards(inwards) {
	float orientation = 1;
	if (inwards)
		orientation = -1;

	std::vector<mesh::vertex> car_vertices{
		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.509675f, 0.765398f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.435423f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.435423f, 0.699437f}
		},

		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z}, {0.0000f * orientation, 0.6641f * orientation, 0.7476f * orientation},
			{0.353780f, 0.217678f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, 0.7476f * orientation}, {0.155895f, 0.118359f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {0.0000f * orientation, 0.6641f * orientation, 0.7476f * orientation},
			{0.353780f, 0.118359f}
		},

		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.000000f, 0.897322f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.699437f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.963284f}
		},

		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, -0.7476f * orientation}, {0.155895f, 0.349601f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, -0.7476f * orientation}, {0.353780f, 0.448921f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, -0.7476f * orientation}, {0.155895f, 0.448921f}
		},

		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.963284f}
		},

		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z}, {0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.217678f}
		},
		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.349601f}
		},
		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.217678f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.448921f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.533357f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.448921f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.963284f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.699437f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.963284f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.118359f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z}, {0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.081643f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.118359f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.435423f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.699437f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, -1.0000f * orientation}, {0.155895f, 0.533357f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, -1.0000f * orientation}, {0.353780f, 0.615000f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, -1.0000f * orientation}, {0.155895f, 0.615000f}
		},

		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.435423f, 0.615000f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.435423f, 0.699437f}
		},

		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.615000f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.699437f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.699437f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.615000f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.699437f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z}, {0.0000f * orientation, 0.0000f * orientation, 1.0000f * orientation},
			{0.353780f, 0.081643f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, 1.0000f * orientation}, {0.155895f, 0.000000f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z}, {0.0000f * orientation, 0.0000f * orientation, 1.0000f * orientation},
			{0.353780f, 0.000000f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.435423f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 1.000000f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.963284f}
		},

		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.963284f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 1.000000f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.963284f}
		},

		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.963284f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 1.000000f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.963284f}
		},

		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.509675f, 0.765398f}
		},
		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.509675f, 0.897322f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.435423f, 0.963284f}
		},

		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z}, {0.0000f * orientation, 0.6641f * orientation, 0.7476f * orientation},
			{0.353780f, 0.217678f}
		},
		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, 0.7476f * orientation}, {0.155895f, 0.217678f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, 0.7476f * orientation}, {0.155895f, 0.118359f}
		},

		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.000000f, 0.897322f}
		},
		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.000000f, 0.765398f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.699437f}
		},

		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, -0.7476f * orientation}, {0.155895f, 0.349601f}
		},
		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, -0.7476f * orientation}, {0.353780f, 0.349602f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, 0.6641f * orientation, -0.7476f * orientation}, {0.353780f, 0.448921f}
		},

		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.963284f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.699437f}
		},

		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, 2.000000f * m_half_extents.z}, {0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.217678f}
		},
		{
			{3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.349602f}
		},
		{
			{-3.000000f * m_half_extents.x, 4.726816f * m_half_extents.y, -2.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.349601f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.448921f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.533357f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.533357f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.963284f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.699437f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.699437f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.118359f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.081643f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z}, {0.0000f * orientation, 1.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.081643f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.435423f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.435423f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 0.963284f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, -1.0000f * orientation}, {0.155895f, 0.533357f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, -1.0000f * orientation}, {0.353780f, 0.533357f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, -1.0000f * orientation}, {0.353780f, 0.615000f}
		},

		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.699437f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.615000f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475450f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.435423f, 0.615000f}
		},

		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.699437f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.615000f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.615000f}
		},

		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.699437f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 0.615000f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, -6.560168f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.615000f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z}, {0.0000f * orientation, 0.0000f * orientation, 1.0000f * orientation},
			{0.353780f, 0.081643f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, 1.0000f * orientation}, {0.155895f, 0.081643f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, 0.0000f * orientation, 1.0000f * orientation}, {0.155895f, 0.000000f}
		},

		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 4.000000f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.435423f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.435423f, 1.000000f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z}, {1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation},
			{0.353780f, 1.000000f}
		},

		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 0.963284f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.155895f, 1.000000f}
		},
		{
			{-3.000000f * m_half_extents.x, 2.475451f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{-1.0000f * orientation, 0.0000f * orientation, 0.0000f * orientation}, {0.074252f, 1.000000f}
		},

		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 4.000000f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 0.963284f}
		},
		{
			{3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.353780f, 1.000000f}
		},
		{
			{-3.000000f * m_half_extents.x, 0.000000f * m_half_extents.y, 5.113251f * m_half_extents.z},
			{0.0000f * orientation, -1.0000f * orientation, 0.0000f * orientation}, {0.155895f, 1.000000f}
		},

	};
	const std::vector<uint32_t> car_indices{

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
		84, 85, 86, 87, 88, 89,
		90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101,
		102, 103, 104, 105, 106, 107,
	};

	m_mesh = engine::mesh::create(car_vertices, car_indices);
}

engine::car::~car() {}

engine::ref<engine::car> engine::car::create(glm::vec3 half_extents, bool inwards) {
	return std::make_shared<engine::car>(half_extents, inwards);
}
