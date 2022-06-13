#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a cuboid object with a mesh of a specified size
	class cuboid
	{
	public:
		/// \brief Constructor
		cuboid(glm::vec3 half_extents, bool inwards);

		/// \brief Destructor
		~cuboid();

		/// \brief Getter methods
		glm::vec3 half_extents() const { return m_half_extents; }
		bool inwards() const { return m_inwards; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<cuboid> create(glm::vec3 half_extents, bool inwards);
	private:
		/// \brief Fields
		// Dimensions of the cuboid specified by a vector of half-extents. Cuboid is centered on the origin in local coordinates.
		glm::vec3 m_half_extents;

		// Boolean recording if the cube should be inverted (used in skybox)
		bool m_inwards;

		ref<engine::mesh> m_mesh;
	};
}
