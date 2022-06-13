#include "pch.h"
#include "heightmap.h"
#include <engine.h>
#include "stb_image.h"


engine::heightmap::heightmap(std::string sTerrainFilename, std::string sTextureFilename, float terrainSizeX, float terrainSizeZ, glm::vec3 origin, float terrainHeightScale) {
	// Load image
	BYTE* bDataPointer;

	if (image_bytes(sTerrainFilename, &bDataPointer) == true) {
		m_origin = origin;
		m_terrain_size_x = terrainSizeX;
		m_terrain_size_z = terrainSizeZ;

		// Allocate memory and initialize to store the image
		m_heightMap = new float[(size_t)m_width * m_height];
		if (m_heightMap != NULL) {
			// Clear the heightmap
			memset(m_heightMap, 0, (size_t)m_width * m_height * sizeof(float));

			// Form mesh
			float halfSizeX = m_width / 2.0f;
			float halfSizeY = m_height / 2.0f;

			uint32_t X = 1;
			uint32_t Z = m_width;
			uint32_t triangleId = 0;
			for (uint32_t z = 0; z < m_height; z++) {
				for (uint32_t x = 0; x < m_width; x++) {
					uint32_t index = x + z * m_width;

					// Retreive the colour from the terrain image, and set the normalized height in the range [0, 1]
					float grayScale = (bDataPointer[index * 3] + bDataPointer[index * 3 + 1] + bDataPointer[index * 3 + 2]) / 3.0f;
					float height = (grayScale - 128.0f) / 128.0f;

					// Make a point based on this pixel position.  Then, transform so that the mesh has the correct size and origin
					// This transforms a point in image coordinates to world coordinates
					glm::vec3 pos_image = glm::vec3((float)x, height, (float)z);
					glm::vec3 pos = image_to_world_coord(pos_image);

					// Scale the terrain and store for later
					pos.y *= terrainHeightScale;
					m_heightMap[index] = pos.y;

					glm::vec3 normal = glm::vec3((float)x, height, (float)z);

					glm::vec2 texture = glm::vec2();

					// Store the point in a vector
					engine::mesh::vertex v = engine::mesh::vertex(pos, normal, texture);
					m_vertices.push_back(v);
				}
			}

			// Form triangles from successive rows of the image
			for (uint32_t z = 0; z < m_height - 1; z++) {
				for (uint32_t x = 0; x < m_width - 1; x++) {
					int index = x + z * m_width;
					m_indices.push_back(index);
					m_indices.push_back(index + X + Z);
					m_indices.push_back(index + X);

					m_indices.push_back(index);
					m_indices.push_back(index + Z);
					m_indices.push_back(index + X + Z);
				}
			}

			// Now we must fill the onTriangle list
			m_on_triangle.resize(m_vertices.size());
			uint32_t numTriangles = (unsigned int)(m_indices.size() / 3);
			for (uint32_t t = 0; t < numTriangles; t++) {
				m_on_triangle.at(m_indices.at((size_t)t * 3)).id.push_back(t);
				m_on_triangle.at(m_indices.at((size_t)t * 3 + 1)).id.push_back(t);
				m_on_triangle.at(m_indices.at((size_t)t * 3 + 2)).id.push_back(t);
			}

			texture_coords(5.0f, 5.0f);
			normal_coords();

			// Create a face vertex mesh
			m_mesh = engine::mesh::create(m_vertices, m_indices);

			// Load a texture for texture mapping the mesh
			m_texture = engine::texture_2d::create(sTextureFilename, false);
		}
	}
}

engine::heightmap::~heightmap() {}

// Convert a point from image (pixel) coordinates to world coordinates
glm::vec3 engine::heightmap::image_to_world_coord(glm::vec3 p) {
	// Normalize the image point so that it in the range [-1, 1] in x and [-1, 1] in z
	p.x = 2.0f * (p.x / m_width) - 1.0f;
	p.z = 2.0f * (p.z / m_height) - 1.0f;

	// Now scale the point so that the terrain has the right size in x and z
	p.x *= m_terrain_size_x / 2.0f;
	p.z *= m_terrain_size_z / 2.0f;

	// Now translate the point based on the origin passed into the function
	p += m_origin;

	return p;
}

// Convert a point from world coordinates to image (pixel) coordinates
glm::vec3 engine::heightmap::world_to_image_coord(glm::vec3 p) {
	p -= m_origin;

	// Normalize the image point so that it in the range [-1, 1] in x and [-1, 1] in z
	p.x *= 2.0f / m_terrain_size_x;
	p.z *= 2.0f / m_terrain_size_z;

	// Now transform the point so that it is in the range [0, 1] in x and [0, 1] in z
	p.x = (p.x + 1.0f) * (m_width / 2.0f);
	p.z = (p.z + 1.0f) * (m_height / 2.0f);

	return p;

}

bool engine::heightmap::image_bytes(std::string sTerrainFilename, BYTE** bDataPointer) {
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(sTerrainFilename.c_str(), &width, &height, &channels, 0);
	CORE_ASSERT(data, "[gl_texture_2d] Texture not loaded correctly");
	m_width = width;
	m_height = height;
	*bDataPointer = data; // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == NULL || width == 0 || height == 0)
		return false;

	return true;
}

// For a point p in world coordinates, return the height of the terrain
float engine::heightmap::ground_height(glm::vec3 p) {
	// Undo the transformation going from image coordinates to world coordinates
	glm::vec3 pImage = world_to_image_coord(p);
	// Bilinear interpolation.  
	uint32_t xl = (uint32_t)floor(pImage.x);
	uint32_t zl = (uint32_t)floor(pImage.z);
	// Check if the position is in the region of the heightmap
	if (xl < 0 || xl >= m_width - 1 || zl < 0 || zl >= m_height - 1)
		return 0.0f;
	// Get the indices of four pixels around the current point 
	uint32_t indexll = xl + zl * m_width;
	uint32_t indexlr = (xl + 1) + zl * m_width;
	uint32_t indexul = xl + (zl + 1) * m_width;
	uint32_t indexur = (xl + 1) + (zl + 1) * m_width;
	// Interpolation amounts in x and z
	float dx = pImage.x - xl;
	float dz = pImage.z - zl;
	// Interpolate -- first in x and and then in z
	float a = (1 - dx) * m_heightMap[indexll] + dx * m_heightMap[indexlr];
	float b = (1 - dx) * m_heightMap[indexul] + dx * m_heightMap[indexur];
	float c = (1 - dz) * a + dz * b;
	return c;
}

engine::ref<engine::heightmap> engine::heightmap::create(std::string sTerrainFilename, std::string sTextureFilename, float terrainSizeX, float terrainSizeZ, glm::vec3 origin,
                                                         float       terrainHeightScale) {
	return std::make_shared<engine::heightmap>(sTerrainFilename, sTextureFilename, terrainSizeX, terrainSizeZ, origin, terrainHeightScale);
}

void engine::heightmap::texture_coords(float x_scale, float z_scale) {
	// Set texture coords based on the x and z coordinates
	for (uint32_t i = 0; i < m_vertices.size(); i++) {
		m_vertices.at(i).tex_coords.x = m_vertices.at(i).position.x / x_scale;
		m_vertices.at(i).tex_coords.y = m_vertices.at(i).position.z / z_scale;
	}
}

void engine::heightmap::normal_coords() {
	for (uint32_t i = 0; i < m_vertices.size(); i++) {
		engine::mesh::vertex v = m_vertices.at(i);

		// Get normal for each triangle around v in one ring neighbourhood
		glm::vec3 normal_sum = glm::vec3(0, 0, 0);
		uint32_t  face_count = 0;
		for (uint32_t j = 0; j < m_on_triangle[i].id.size(); j++) {
			int tId = m_on_triangle[i].id[j];
			normal_sum += compute_normal(tId);
			face_count++;
		}
		normal_sum = normal_sum / (float)face_count;
		glm::normalize(normal_sum);

		m_vertices[i].normal.x = normal_sum.x;
		m_vertices[i].normal.y = normal_sum.y;
		m_vertices[i].normal.z = normal_sum.z;

	}
}

// Compute the normal of a triangle using the cross product
glm::vec3 engine::heightmap::compute_normal(uint32_t tId) {
	engine::mesh::vertex v0,     v1, v2;
	glm::vec3            normal, p,  q;

	v0 = m_vertices.at(m_indices.at(3 * (size_t)tId));
	v1 = m_vertices.at(m_indices.at(3 * (size_t)tId + 1));
	v2 = m_vertices.at(m_indices.at(3 * (size_t)tId + 2));

	p = glm::vec3(v1.position.x, v1.position.y, v1.position.z) - glm::vec3(v0.position.x, v0.position.y, v0.position.z);
	q = glm::vec3(v2.position.x, v2.position.y, v2.position.z) - glm::vec3(v0.position.x, v0.position.y, v0.position.z);
	normal = glm::cross(p, q); // Cross product
	glm::normalize(normal);

	return normal;
}
