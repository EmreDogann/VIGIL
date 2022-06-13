#pragma once
#include "event.h"


namespace engine
{
    //=========================================================================

    class ENGINE_API key_event : public event
    {
    public:
        int32_t key_code() const { return m_key_code; }
        EVENT_CLASS_CATEGORY(event_category_keyboard | event_category_input)

    protected:
        key_event(int32_t keycode)
            : m_key_code(keycode)
        {
        }

        int32_t m_key_code;
    };

    //=========================================================================

    class ENGINE_API key_pressed_event : public key_event
    {
    public:
        key_pressed_event(int32_t keycode, int32_t repeatCount)
            : key_event(keycode), m_repeats_count(repeatCount)
        {
        }
        
        int32_t repeats_count() const { return m_repeats_count; }

        std::string to_string() const override
        {
            std::stringstream ss;
            ss << "key_pressed_event: " << m_key_code << " (" << m_repeats_count << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(key_pressed)

    private:
        int32_t m_repeats_count;
    };

    //=========================================================================

    class ENGINE_API key_released_event : public key_event
    {
    public:
        key_released_event(int32_t keycode)
            : key_event(keycode)
        {
        }

        std::string to_string() const override
        {
            std::stringstream ss;
            ss << "key_released_event: " << m_key_code;
            return ss.str();
        }

        EVENT_CLASS_TYPE(key_released)
    };

    //=========================================================================

    /// \brief Represents the char value of the key pressed. 
    /// [Most commonly known as char_event in windows]
    class ENGINE_API key_typed_event : public key_event
    {
    public:
        key_typed_event(int32_t keycode)
            : key_event(keycode)
        {
        }

        std::string to_string() const override
        {
            std::stringstream ss;
            ss << "key_typed_event: " << m_key_code;
            return ss.str();
        }

        EVENT_CLASS_TYPE(key_typed)
    };

    //=========================================================================
}
