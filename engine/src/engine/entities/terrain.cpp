#include "pch.h"
#include "terrain.h"

engine::terrain::terrain(float length, float depth, float width) : cuboid(glm::vec3(length, depth, width), false), m_length(length), m_depth(depth), m_width(width) {}

engine::terrain::~terrain() {}

engine::ref<engine::terrain> engine::terrain::create(float length, float depth, float width)
{
	return std::make_shared<engine::terrain>(length, depth, width);
}
