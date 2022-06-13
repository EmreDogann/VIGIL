#pragma once
#include "engine.h"

namespace engine
{

	/// \brief Class creating a grid object of a specified size
	class grid
	{
	public:
		/// \brief Constructor
		grid(uint32_t length, uint32_t width, uint32_t step);
		/// \brief Destructor
		~grid();

		/// \brief Getter methods
		float width() const { return m_width; }
		float length() const { return m_length; }
		float step() const { return m_step; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		void on_render(const ref<shader>& shader);

		static ref<grid> create(uint32_t length, uint32_t width, uint32_t step);
	private:
		/// \brief Fields
		// grid's width
		float m_width;
		// grid's length
		float m_length;
		// grid's step size
		float m_step;
		// grid's mesh
		ref<engine::mesh> m_mesh;
	};
}
