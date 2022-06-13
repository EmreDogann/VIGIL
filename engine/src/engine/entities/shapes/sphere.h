#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a sphere object with a vertex array of a specified size and tesselation
	class sphere final
	{
	public:
        // TODO - objects (any object for now, more edge cases will be explained on the way) should be passed as !!! const& !!! [one exception here is the smart pointer "ref"]
		/// \brief Constructor
		sphere(uint32_t stacks, uint32_t slices, float radius);

		/// \brief Destructor
		~sphere();

        /// \brief Getter methods
		uint32_t stacks() const { return m_stacks; }
		uint32_t slices() const { return m_slices; }
		float radius() const { return m_radius; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<sphere> create(uint32_t stacks, uint32_t slices, float radius);

	private:
		/// \brief Fields
		// Vertical tessellation level of the sphere
		uint32_t m_stacks;
		// horizontal tessellation level of the sphere
		uint32_t m_slices;
		// Sphere's radius
		float m_radius;
		// Sphere's mesh
		ref<engine::mesh> m_mesh;
	};
}
