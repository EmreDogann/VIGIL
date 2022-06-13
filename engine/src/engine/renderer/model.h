#pragma once

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace engine
{
    class mesh;
    class texture_2d;

    /// \brief
    class model final
    {
    public:
        model(const std::string& path);
        ~model();

		///\brief - Getters
		glm::vec3 size() const { return m_size; }
		glm::vec3 offset() const { return m_offset; }
		std::vector <ref<engine::mesh>> meshes() const { return m_meshes; }
		std::vector <ref<engine::texture_2d>> textures() const { return m_textures; }

		// create the object's smart pointer 
		static std::shared_ptr<engine::model> engine::model::create(const std::string& path);
		
    private: // methods
        void process_node(aiNode* node, const aiScene* scene);
        ref<engine::mesh> process_mesh(aiMesh* mesh, const aiScene* scene);
        std::vector<ref<texture_2d>> load_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name) const;
		
        ///\brief - compares the values of the vertex that is being loaded with min and max values
		/// if any coordinate is smaller than min or bigger than max, record it as new min/max value
		void min_max_compare(const glm::vec3& point);

    private: // fields
        std::string m_directory;
        std::string m_path;

		// model's meshes
		std::vector<ref<mesh>> m_meshes;

		// model's textures
		std::vector<ref<texture_2d>> m_textures;

		// min values of the model in x, y and z coordinates
		glm::vec3 m_min_point;

		// max values of the model in x, y and z coordinates
		glm::vec3 m_max_point;

		// switch that checks if the current point is the first point loaded for the model
		bool m_first_point = true;

		// dimensions of the object in x, y and z
		glm::vec3 m_size;

		// offset of the model from the origin of the local coordinates
		glm::vec3 m_offset;
    };
}
