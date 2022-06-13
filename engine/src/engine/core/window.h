#pragma once
#include "pch.h"

#include "engine/core.h"
#include "engine/events/event.h"

namespace engine
{
	struct window_props
	{
		std::string m_title;
		uint32_t    m_width;
		uint32_t    m_height;
		uint32_t    m_pos_x;
		uint32_t    m_pos_y;
		bool		m_fullscreen;
		bool        m_vsync;

		window_props(std::string const& title = "engine",
			uint32_t width = 1280,
			uint32_t height = 720,
			uint32_t pos_x = 0,
			uint32_t pos_y = 0,
			bool fullscreen = false,
			bool vsync = false)
			: m_title(title), m_width(width), m_height(height), m_pos_x(pos_x), m_pos_y(pos_y), m_fullscreen(fullscreen), m_vsync(vsync)
		{
		}
	};

	/// \brief Interface representing a desktop system based window.
	class ENGINE_API window
	{
	public:
		static scope<window> create(window_props const& props = window_props());

		//---------------------------------------------------------------------

		using event_callback_fn = std::function<void(event&)>;

		virtual ~window() = default;

		//---------------------------------------------------------------------

		virtual void on_update() = 0;

		//---------------------------------------------------------------------

		virtual void event_callback(event_callback_fn const& callback) = 0;
		virtual void vsync(bool enabled) = 0;
		virtual bool vsync() = 0;
		virtual void show_mouse_cursor() = 0;
		virtual void hide_mouse_cursor() = 0;
		virtual bool is_cursor_visible() const = 0;

		virtual void* native_window() const = 0;

		//---------------------------------------------------------------------

		virtual uint32_t width()  const = 0;
		virtual uint32_t height() const = 0;
	};
}
