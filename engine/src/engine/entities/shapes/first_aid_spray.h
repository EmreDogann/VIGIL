#pragma once

namespace engine {
	class mesh;

	class first_aid_spray {
	public:
		/// \brief Constructor
		first_aid_spray(glm::vec3 half_extents, bool inwards);

		/// \brief Destructor
		~first_aid_spray();

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

		static ref<first_aid_spray> create(glm::vec3 half_extents, bool inwards);
	private:
		glm::vec3 m_half_extents;

		bool m_inwards;

		ref<engine::mesh> m_mesh;
	};
}
