#pragma once

namespace engine {
	class mesh;

	class ammo_box {
	public:
		/// \brief Constructor
		ammo_box(glm::vec3 half_extents, bool inwards);

		/// \brief Destructor
		~ammo_box();

		/// \brief Getter methods
		glm::vec3 half_extents() const {
			return m_half_extents;
		}

		bool inwards() const {
			return m_inwards;
		}

		ref<engine::mesh> mesh() const {
			return m_mesh;
		}

		static ref<ammo_box> create(glm::vec3 half_extents, bool inwards);
	private:
		glm::vec3 m_half_extents;

		bool m_inwards;

		ref<engine::mesh> m_mesh;
	};
}
