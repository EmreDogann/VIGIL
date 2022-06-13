#pragma once

#include "engine/core.h"
#include "layer.h"

namespace engine {
	/// \brief Wrapper around a vector of layers.
	/// It's not an actual stack as we need to push layers in the middle of the stack.
	class layers_stack {
	public:
		layers_stack() = default;
		~layers_stack();

		/// \brief Adds the layer to the first half of the layers_stack.
		void push_layer(layer* layer);
		/// \brief Adds the overlay to the second half of the layers_stack.
		/// Every new overlay will be added to the very end of the 
		void push_overlay(layer* overlay);
		/// \brief Removes the layer from the layers_stack. Layer won't be deleted.
		void pop_layer(layer* layer);
		/// \brief Removes the overlay from the layers_stack. Overlay won't be deleted.
		void pop_overlay(layer* overlay);

		// To iterate through a range-based for-loop [i.e. for( auto elem : elements)]
		/// \brief Implicit implementation of .begin()
		std::vector<layer*>::iterator begin() {
			return m_layers.begin();
		}

		/// \brief Implicit implementation of .end()
		std::vector<layer*>::iterator end() {
			return m_layers.end();
		}

		///  \brief Get the last layer in the stack.
		layer* back() {
			return m_layers.back();
		}

	private:
		std::vector<layer*> m_layers;
		uint32_t            m_layers_insert_index{0};
	};
}
