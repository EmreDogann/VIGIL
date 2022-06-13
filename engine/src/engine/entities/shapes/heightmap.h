#pragma once
#include "engine/renderer/mesh.h"

namespace engine
{
	class texture_2d;

	typedef struct {
		std::vector<uint32_t> id;	// list of triangle IDs 
	} triangle_list;

	class heightmap
	{
	public:
		heightmap(std::string sTerrainFilename, std::string sTextureFilename, float terrainSizeX, float terrainSizeZ, glm::vec3 origin, float terrainHeightScale);
		~heightmap();
		static ref<heightmap> create(std::string sTerrainFilename, std::string sTextureFilename, float terrainSizeX, float terrainSizeZ, glm::vec3 origin, float terrainHeightScale);
		float ground_height(glm::vec3 p);
		ref<mesh> mesh() { return m_mesh; };
		ref<texture_2d > texture() { return m_texture; };
		glm::vec2 terrain_size() { return glm::vec2(m_terrain_size_x, m_terrain_size_z); };
		bool image_bytes(std::string sTerrainFilename, BYTE** bDataPointer);

	private:
		uint32_t m_width{0}, m_height{0};
		std::vector<mesh::vertex> m_vertices;
		std::vector<unsigned int> m_indices;
		std::vector<triangle_list> m_on_triangle;	// For each vertex, stores a list of triangle IDs saying which triangles the vertex is on
		float* m_heightMap;
		ref<engine::mesh> m_mesh;
		float m_terrain_size_x, m_terrain_size_z;
		glm::vec3 m_origin;
		ref<texture_2d> m_texture;

		glm::vec3 world_to_image_coord(glm::vec3 p);
		glm::vec3 image_to_world_coord(glm::vec3 p);
		void texture_coords(float x_scale, float z_scale);
		void normal_coords();
		glm::vec3 compute_normal(uint32_t tId);
	};
}
