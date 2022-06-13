#pragma once

#include "engine/core.h"
#include "engine/events/event.h"
#include "engine/core/timestep.h"

namespace engine {
	class shader;

	/// \brief Interface/base-class from which implement the layers in your applicaiton.
	class ENGINE_API layer {
	public:
		layer(const std::string& name = "Layer");
		virtual ~layer();

		/// \brief Runs when a layer is pushed onto the layer stack
		virtual void on_attach() {}
		/// \brief Runs when a layer is popped from the layer stack
		virtual void on_detach() {}
		/// \brief Runs when application calls the on_update method.
		/// Should happen every frame.
		virtual void on_update(const timestep& timestep) {}
		/// \brief Render method.
		virtual void on_render() {}
		/// \brief Runs whenever an event gets sent to the layer.
		virtual void on_event(event& event) {}
		/// \brief Runs transition to the next scene when layer is detached.
		virtual void detach_scene_transition(const engine::timestep& time_step) {}
		/// \brief Runs transition of the new scene when layer is attached.
		virtual void attach_scene_transition(const engine::timestep& time_step) {}
		/// \brief Default parameters to set shader.
		virtual void set_default_shader_parameters() {}

		/// \brief Gives you the name of the layer [for debug]
		const std::string& name() const {
			return m_debug_name;
		}

		bool is_imgui() const {
			return m_imgui;
		}

		void set_is_layer_active(bool val) {
			is_layer_active = val;
		}

		bool get_is_layer_active() {
			return is_layer_active;
		}

		void set_is_layer_frozen(bool val) {
			is_layer_frozen = val;
		}

		bool get_is_layer_frozen() {
			return is_layer_frozen;
		}

		void set_attach_transition_in_progress(bool val) {
			attach_transition_in_progress = val;
		}

		bool get_attach_transition_in_progress() {
			return attach_transition_in_progress;
		}

		void set_detach_transition_in_progress(bool val) {
			detach_transition_in_progress = val;
		}

		bool get_detach_transition_in_progress() {
			return detach_transition_in_progress;
		}

	protected:
		std::string m_debug_name{};
		/// \brief Callback function to invoke when attach/detach transition is finished.
		std::function<void()> on_transition_finish = []()
		{};

		/// \brief Flag to distinguish between imgui and engine rendering calls.
		bool m_imgui{false};

		// The difference between these two is, all layers which isn't marked as active won't have its on_update or on_render called.
		// But, a layer that is marked as frozen won't have its on_update called but will still be rendered.
		bool is_layer_active = false;
		bool is_layer_frozen = false;

		bool  attach_transition_in_progress = false;
		bool  detach_transition_in_progress = false;
		float transition_timer;
	};
}
