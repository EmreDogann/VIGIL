#pragma once
#include "engine/core/window.h"
#include "engine/renderer/graphics_context.h"
#include "GLFW/glfw3.h"

namespace engine
{
    class win_window : public window
    {
        //---------------------------------------------------------------------

    public:
        win_window(window_props const& props);
        virtual ~win_window();

        //---------------------------------------------------------------------

        void on_update() override;

        //---------------------------------------------------------------------

        void event_callback(event_callback_fn const& callback) override { m_data.event_callback = callback; }
        void vsync(bool p_enabled) override;
        bool vsync() override;
        void show_mouse_cursor() override;
        void hide_mouse_cursor() override;
        bool is_cursor_visible() const override;

        virtual void* native_window() const override { return m_window; }

        //---------------------------------------------------------------------

        uint32_t width() const override { return m_data.width; }
        uint32_t height() const override { return m_data.height; }

        //---------------------------------------------------------------------

    private:
        virtual void init(window_props const& props);
        virtual void shut_down();

    private:
        GLFWwindow* m_window = nullptr;
		graphics_context* m_graphics_context = nullptr;

        /// \brief struct to pass to the glfw callback user_ptr
        struct window_data
        {
            std::string title;
            uint32_t width;
            uint32_t height;
            float last_mouse_x;
            float last_mouse_y;
            bool vsync;
            bool is_mouse_cursor_visible;

            event_callback_fn event_callback;
        };

        window_data m_data;
    };

}
