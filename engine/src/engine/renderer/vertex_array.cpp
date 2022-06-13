#include "pch.h"
#include "vertex_array.h"
#include "renderer.h"

#include "platform/opengl/gl_vertex_array.h"

engine::ref<engine::vertex_array> engine::vertex_array::create()
{
    switch (renderer::api())
    {
        case e_api_type::none: CORE_ASSERT(false, "[vertex_array] e_renderer_api::none currently not supported!"); return nullptr;
        case e_api_type::open_gl: return std::make_shared<gl_vertex_array>();
    }

    CORE_ASSERT(false, "[vertex_array] Unknown renderer api!");
    return nullptr;
}
