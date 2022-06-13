#pragma once
namespace engine
{
	class shader;

	class material
	{
	public:
		material(const float shininess, const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, float transparency);
		~material();

		float shininess() const { return m_shininess; }
		glm::vec3 ambient() const { return m_ambient; }
		glm::vec3 diffuse() const { return m_diffuse; }
		glm::vec3 specular() const { return m_specular; }
		float transparency() const { return m_transparency; }

		void set_shininess(float shininess) { m_shininess = shininess; }
		void set_ambient(glm::vec3 ambient) { m_ambient = ambient; }
		void set_diffuse(glm::vec3 diffuse) { m_diffuse = diffuse; }
		void set_specular(glm::vec3 specular) { m_specular = specular; }
		void set_transparency(float transparency) { m_transparency = transparency; }

		static ref<material> create(const float shininess,
			const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, float transparency);

		void submit(const ref<shader> shader);

	private:
		float m_shininess;
		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;
		float m_transparency;
	};
}
