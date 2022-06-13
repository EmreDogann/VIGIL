#pragma once

#include "engine/core.h"

namespace engine
{
    /// \brief Base class for input polling.
    /// input polling allows to retrieve input states at any time.
    class ENGINE_API input
    {
    protected:
        input() = default;
    public:
        input(const input&) = delete;
        input& operator=(const input&) = delete;

        /// \brief Returns if the specified key is being pressed.
        static bool key_pressed(int p_key_code) { return s_instance->key_pressed_impl(p_key_code); }

        /// \brief Returns if the specified mouse button is being pressed.
        static bool mouse_button_pressed(int p_button) { return s_instance->mouse_button_pressed_impl(p_button); }

        /// \brief Returns the mouse position as a pair of float values.
        static std::pair<float, float> mouse_position() { return s_instance->mouse_position_impl(); }
        /// \brief Returns the x coordinate of the mouse position.
        static float mouse_x() { return s_instance->mouse_x_impl(); }
        /// \brief Returns the y coordinate of the mouse position.
        static float mouse_y() { return s_instance->mouse_y_impl(); }

    protected:
        virtual bool key_pressed_impl(int key_code) const = 0;
        virtual bool mouse_button_pressed_impl(int key_code) const = 0;
        virtual std::pair<float, float> mouse_position_impl() const = 0;
        virtual float mouse_x_impl() const = 0;
        virtual float mouse_y_impl() const = 0;

    private:
        static input* s_instance;
    };
}