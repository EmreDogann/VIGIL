#pragma once
#include "renderer_api.h"

namespace engine
{
    /// \brief
    class render_command
    {
    public:
        static void init()
        {
            renderer_api::init();
        }

        static void resize_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            renderer_api::resize_viewport(x,y,width, height);
        }

        static void clear_color(const glm::vec4 &color)
        {
            renderer_api::clear_color(color);
        }

        static void clear()
        {
            renderer_api::clear();
        }

        static void toggle_wireframe()
        {
            static bool is_wireframe = true;
            if(is_wireframe)
                renderer_api::enable_wireframe();
            else
                renderer_api::disable_wireframe();

            is_wireframe = !is_wireframe;
        }

        static void primitive_type(const e_primitive_type& type)
        {
            renderer_api::primitive_type(type);
        }

        static void submit(const ref<vertex_array>& vertex_array)
        {
            renderer_api::draw_indexed(vertex_array);
        }

    };
}
