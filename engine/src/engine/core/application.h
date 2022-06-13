#pragma once

#include "engine/core.h"

#include "window.h"
#include "engine/core/layers_stack.h"
#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/core/timestep.h"
#include "engine/utils/audio_manager.h"


namespace engine {
	class ENGINE_API application {
	public:
		application();
		virtual ~application();

		/// \brief
		void run();

		/// \brief
		virtual void on_event(event& event);

		/// \brief Adds a layer to the stack.
		void push_layer(layer* layer);
		/// \brief Adds a overlay to the stack.
		void push_overlay(layer* overlay);

		/// \brief Adds a layer to the buffer stack to be added to the main stack later on.
		void push_layer_buffer(layer* layer);
		/// \brief Adds a overlay to the buffer stack.
		void push_overlay_buffer(layer* overlay);

		/// \brief Adds a layer to the buffer stack to be popped on the main stack later on.
		void pop_layer_buffer(layer* layer);
		/// \brief Adds a overlay to the buffer stack.
		void pop_overlay_buffer(layer* overlay);

		/// \brief Returns a reference to the application window.
		static window& window() {
			return *(s_instance->m_window);
		}

		/// \brief Returns a reference to the application.
		static application& instance() {
			return *s_instance;
		}

		/// \brief Returns a reference to the application audio manager.
		static engine::ref<engine::audio_manager> audio() {
			return s_instance->m_audio_manager;
		}

		/// \brief Returns a reference to the playtime.
		static float& play_time() {
			return s_play_time;
		}

	private:
		bool on_window_close(window_closed_event& e);
		bool on_window_resized(window_resize_event& e);

	private:
		scope<engine::window>              m_window;
		engine::ref<engine::audio_manager> m_audio_manager;
		layers_stack                       m_layers_stack;
		// 0: layer, 1: overlay
		std::vector<std::pair<int, layer*>> m_layers_stack_buffer;
		// Buffer stack to pop.
		std::vector<std::pair<int, layer*>> m_layers_pop_stack_buffer;

	private:
		static application* s_instance;
		static bool         s_running;
		static bool         s_minimized;
		static float        s_play_time;

	public:
		static void exit();

	};


	application* create_application();
}
