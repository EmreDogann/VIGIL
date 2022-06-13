#include "pch.h"
#include "win_window.h"

#include "engine/events/application_event.h"
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"

#include "engine/core.h"
#include "platform/opengl/gl_context.h"
#include "GLFW/glfw3.h"
#include "engine/core/input.h"

//=============================================================================

namespace engine
{
    static bool s_glfw_initialized = false;

    static void glfw_error_callback(int error_code, const char * description)
    {
        LOG_CORE_ERROR("GLFW Error ({0}): {1}", error_code, description);
    }
}

engine::scope<engine::window> engine::window::create(window_props const& props)
{
    return std::make_unique<win_window>(props);
}

engine::win_window::win_window(window_props const& props)
{
    init(props);
}

engine::win_window::~win_window()
{
    shut_down();
}

void engine::win_window::on_update()
{
	glfwPollEvents();
	m_graphics_context->swap_buffers();
}

void engine::win_window::vsync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_data.vsync = enabled;
}

bool engine::win_window::vsync()
{
    return m_data.vsync;
}

void engine::win_window::show_mouse_cursor()
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_data.is_mouse_cursor_visible = true;
}

void engine::win_window::hide_mouse_cursor()
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_data.is_mouse_cursor_visible = false;
}

bool engine::win_window::is_cursor_visible() const
{
    return m_data.is_mouse_cursor_visible;
}

void engine::win_window::init(window_props const& props)
{
    m_data.title = props.m_title;
    m_data.width = props.m_width;
    m_data.height = props.m_height;
    m_data.last_mouse_x = m_data.width  * 0.5f;
    m_data.last_mouse_y = m_data.height * 0.5f;

    LOG_CORE_INFO("Creating window {0} [{1},{2}]", props.m_title, props.m_width, props.m_height);

    if (!s_glfw_initialized)
    {
        int32_t success = glfwInit();
        CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(glfw_error_callback);
        s_glfw_initialized = true;
    }

	if (props.m_fullscreen)
	{
		int* count = new int;
		m_window = glfwCreateWindow(
			static_cast<int>(props.m_width),
			static_cast<int>(props.m_height),
			props.m_title.c_str(),
			glfwGetMonitors(count)[0], nullptr);
	}
	else
	{
		m_window = glfwCreateWindow(
			static_cast<int>(props.m_width),
			static_cast<int>(props.m_height),
			props.m_title.c_str(),
			nullptr, nullptr);
	}

    glfwMakeContextCurrent(m_window);

	m_graphics_context = new gl_context(m_window);
	m_graphics_context->init();

	glfwSetWindowPos(m_window, props.m_pos_x, props.m_pos_y);

	// we're telling glfw to pass the window_data struct to all the defined callbacks
	// so that we can work with our defined data.
	glfwSetWindowUserPointer(m_window, &m_data);
	vsync(true);

    // GLFW callbacks
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int32_t width, int32_t height)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));
        data.width = width;
        data.height = height;

        window_resize_event event(width, height);
        data.event_callback(event);
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));
        window_closed_event event;
        data.event_callback(event);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));

        switch (action)
        {
            case GLFW_PRESS:
            {
                key_pressed_event event(key, 0);
                data.event_callback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                key_released_event event(key);
                data.event_callback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                key_pressed_event event(key, 1);
                data.event_callback(event);
                break;
            }
        }
    });
    
    glfwSetCharCallback(m_window, [](GLFWwindow* window, uint32_t key)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));

        key_typed_event event(key);
        data.event_callback(event);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));

        switch (action)
        {
            case GLFW_PRESS:
            {
                mouse_button_pressed_event event(button);
                data.event_callback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                mouse_button_released_event event(button);
                data.event_callback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));
        mouse_scrolled_event event(static_cast<float>(xOffset), static_cast<float>(yOffset));
        data.event_callback(event);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x_pos, double y_pos)
    {
        window_data &data = *static_cast<window_data*>(glfwGetWindowUserPointer(window));
        mouse_moved_event event(static_cast<float>(x_pos), static_cast<float>(y_pos));

        if (!data.is_mouse_cursor_visible)
        {
            const float delta_x = static_cast<float>(x_pos) - data.last_mouse_x;
            const float delta_y = data.last_mouse_y - static_cast<float>(y_pos);

            //LOG_CORE_TRACE("Delta -> x: {0} | y: {1}", delta_x, delta_y);
            event = { delta_x, delta_y };
        }

        data.last_mouse_x = static_cast<float>(x_pos);
        data.last_mouse_y = static_cast<float>(y_pos);
        data.event_callback(event);   
    });
}

void engine::win_window::shut_down()
{
    glfwDestroyWindow(m_window);
}
