#include "pch.h"
#include "application.h"
#include "engine/renderer/renderer.h"
#include "GLFW/glfw3.h"
#include "engine/utils/timer.h"

//----------------------------------------------------------------------------- 

engine::application* engine::application::s_instance{nullptr};
bool                 engine::application::s_running{true};
bool                 engine::application::s_minimized{false};
float                engine::application::s_play_time{0.0f};

//----------------------------------------------------------------------------- 

engine::application::application() {
	CORE_ASSERT(!s_instance, "Application already exists!");
	s_instance = this;

	engine::window_props props = engine::window_props("engine", 1920, 1080, 5, 35, false, false);
	m_window = window::create(props);
	m_window->event_callback(BIND_EVENT_FN(application::on_event));

	// Load 
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();

	// --------------- Music ----------------

	m_audio_manager->load_sound("assets/audio/MainMenu_Ambient.wav", engine::sound_type::track, "mainmenu_ambiance");
	m_audio_manager->load_sound("assets/audio/Level1_Ambient.wav", engine::sound_type::track, "level1_ambiance");
	m_audio_manager->load_sound("assets/audio/Level2_Ambient.wav", engine::sound_type::track, "level2_ambiance");
	m_audio_manager->volume("mainmenu_ambiance", 0.3f);
	m_audio_manager->volume("level1_ambiance", 0.3f);
	m_audio_manager->loop("mainmenu_ambiance", false);
	m_audio_manager->loop("level1_ambiance", false);


	// --------------- Transitions ----------

	m_audio_manager->load_sound("assets/audio/Door_Transition.wav", engine::sound_type::track, "door_transition");
	m_audio_manager->loop("door_transition", false);


	// --------------- Footsteps ------------

	m_audio_manager->load_sound("assets/audio/Wood_Footstep1.wav", engine::sound_type::spatialised, "wood_footstep1");
	m_audio_manager->load_sound("assets/audio/Wood_Footstep2.wav", engine::sound_type::spatialised, "wood_footstep2");
	m_audio_manager->load_sound("assets/audio/Wood_Footstep3.wav", engine::sound_type::spatialised, "wood_footstep3");
	m_audio_manager->load_sound("assets/audio/Wood_Footstep4.wav", engine::sound_type::spatialised, "wood_footstep4");

	m_audio_manager->load_sound("assets/audio/Snow_Footstep1.wav", engine::sound_type::spatialised, "snow_footstep1");
	m_audio_manager->load_sound("assets/audio/Snow_Footstep2.wav", engine::sound_type::spatialised, "snow_footstep2");
	m_audio_manager->load_sound("assets/audio/Snow_Footstep3.wav", engine::sound_type::spatialised, "snow_footstep3");
	m_audio_manager->load_sound("assets/audio/Snow_Footstep4.wav", engine::sound_type::spatialised, "snow_footstep4");
	m_audio_manager->load_sound("assets/audio/Snow_Footstep5.wav", engine::sound_type::spatialised, "snow_footstep5");


	// --------------- UI elements ----------

	m_audio_manager->load_sound("assets/audio/UI_Click.wav", engine::sound_type::event, "ui_click");
	m_audio_manager->load_sound("assets/audio/UI_Reveal1.wav", engine::sound_type::event, "ui_reveal1");
	m_audio_manager->load_sound("assets/audio/UI_Reveal2.wav", engine::sound_type::event, "ui_reveal2");


	// --------------- Zombie ---------------

	m_audio_manager->load_sound("assets/audio/Zombie_Attack.wav", engine::sound_type::spatialised, "zombie_attack");
	m_audio_manager->load_sound("assets/audio/Zombie_Dying.wav", engine::sound_type::spatialised, "zombie_dying");
	m_audio_manager->load_sound("assets/audio/Zombie_Groan1.wav", engine::sound_type::spatialised, "zombie_groan1");
	m_audio_manager->load_sound("assets/audio/Zombie_Groan2.wav", engine::sound_type::spatialised, "zombie_groan2");


	// --------------- Player ---------------

	m_audio_manager->load_sound("assets/audio/Gunshot.wav", engine::sound_type::spatialised, "gunshot");
	m_audio_manager->load_sound("assets/audio/Gunshot_Empty.wav", engine::sound_type::spatialised, "gunshot_empty");
	m_audio_manager->load_sound("assets/audio/Player_Hit.wav", engine::sound_type::spatialised, "player_hit");

	renderer::init();
}

engine::application::~application() {}

void engine::application::run() {
	engine::timer gameLoopTimer;
	gameLoopTimer.start();
	while (s_running) {

		// Check if layers need to be removed.
		if (m_layers_pop_stack_buffer.size() > 0) {
			for (std::pair<int, engine::layer*> layer : m_layers_pop_stack_buffer) {
				layer.first ? m_layers_stack.pop_layer(layer.second) : m_layers_stack.pop_overlay(layer.second);
				delete layer.second;
			}
			m_layers_pop_stack_buffer.clear();
		}

		// Check if layers needed to be added.
		if (m_layers_stack_buffer.size() > 0) {
			for (std::pair<int, engine::layer*> layer : m_layers_stack_buffer) {
				layer.first ? m_layers_stack.push_layer(layer.second) : m_layers_stack.push_overlay(layer.second);
			}
			m_layers_stack_buffer.clear();
			gameLoopTimer.reset();
		}

		timestep time_step = (float)gameLoopTimer.elapsed();
		gameLoopTimer.reset();

		s_play_time += time_step; // Keep track of playtime to display on death screen.
		for (auto* layer : m_layers_stack) {
			// Re-activate the top-most layer in the stack.
			if (layer == m_layers_stack.back() && (!m_layers_stack.back()->get_is_layer_active() || m_layers_stack.back()->get_is_layer_frozen())) {
				layer->set_is_layer_active(true);
				layer->set_is_layer_frozen(false);
				layer->set_default_shader_parameters();
				layer->set_attach_transition_in_progress(true);
			}

			// Marking a layer as not active will completely prevent it from being updated or rendered. Marking a layer as frozen will pause the layer's activities but keep rendering it.

			// Only call layer's on_update if not frozen and is marked active.
			if (!layer->get_is_layer_frozen() && layer->get_is_layer_active()) layer->on_update(time_step);

			// Call layer's on_render if it is active.
			if (layer->get_is_layer_active()) {
				if (!application::s_minimized)
					layer->on_render();
			}

		}
		m_window->on_update();
	}
}

void engine::application::on_event(event& event) {
	event_dispatcher dispatcher(event);
	// dispatch event on window X pressed 
	dispatcher.dispatch<window_closed_event>(BIND_EVENT_FN(application::on_window_close));
	dispatcher.dispatch<window_resize_event>(BIND_EVENT_FN(application::on_window_resized));

	//LOG_CORE_TRACE("{0}", event); 

	// Events are executed from top of the stack to bottom (aka end to start of the list) 
	for (auto it = m_layers_stack.end(); it != m_layers_stack.begin();) {
		// Only call layer's event function if it is active and not frozen.
		--it;
		if (!(*it)->get_is_layer_frozen() && (*it)->get_is_layer_active()) {
			(*it)->on_event(event);
			// stop event propagation to next layer if flagged as handled 
			if (event.handled)
				break;
		}
	}
}

void engine::application::push_layer(layer* layer) {
	m_layers_stack.push_layer(layer);
}

void engine::application::push_overlay(layer* overlay) {
	m_layers_stack.push_overlay(overlay);
}

void engine::application::push_layer_buffer(layer* layer) {
	m_layers_stack_buffer.push_back({0, layer});
}

void engine::application::push_overlay_buffer(layer* overlay) {
	m_layers_stack_buffer.push_back({1, overlay});
}

void engine::application::pop_layer_buffer(layer* layer) {
	m_layers_pop_stack_buffer.push_back({0, layer});
}

void engine::application::pop_overlay_buffer(layer* overlay) {
	m_layers_pop_stack_buffer.push_back({1, overlay});
}

bool engine::application::on_window_close(window_closed_event&) {
	exit();
	const bool event_handled = false;
	return event_handled;
}

bool engine::application::on_window_resized(window_resize_event& e) {
	if (e.height() == 0 || e.width() == 0) {
		application::s_minimized = true;
		return false;
	}
	application::s_minimized = false;

	render_command::resize_viewport(0, 0, e.width(), e.height());

	const bool event_handled = false;
	return event_handled;
}

void engine::application::exit() {
	s_running = false;
}
