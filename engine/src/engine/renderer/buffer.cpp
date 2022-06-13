#include "pch.h"
#include "buffer.h"
#include "renderer.h"
#include "platform/opengl/gl_buffer.h"

//=================== vertex buffer =======================================

engine::ref<engine::vertex_buffer>  engine::vertex_buffer::create(const float * vertices, uint32_t size)
{
    switch (renderer::api())
    {
    case e_api_type::none: CORE_ASSERT(false, "[vertex_buffer] e_renderer_api::none currently not supported!"); return nullptr;
    case e_api_type::open_gl: return std::make_shared<gl_vertex_buffer>(vertices, size);
    }

    CORE_ASSERT(false, "[vertex_buffer] Unknown renderer api!");
    return nullptr;
}

//=================== index buffer ========================================

engine::ref<engine::index_buffer> engine::index_buffer::create(const uint32_t* indices, uint32_t count)
{
    switch (renderer::api())
    {
    case e_api_type::none: CORE_ASSERT(false, "[vertex_buffer] e_renderer_api::none currently not supported!"); return nullptr;
    case e_api_type::open_gl: return std::make_shared<gl_index_buffer>(indices, count);
    }

    CORE_ASSERT(false, "[vertex_buffer] Unknown renderer api!");
    return nullptr;
}

//=================== buffer layout =======================================

engine::buffer_layout::buffer_layout(std::initializer_list<buffer_element> elements)
    :m_elements(elements)
{
    calculate_offset_and_stride();
}

const std::vector<engine::buffer_element>& engine::buffer_layout::elements() const
{
    return m_elements;
}

uint32_t engine::buffer_layout::stride() const
{
    return m_stride;
}

void engine::buffer_layout::calculate_offset_and_stride()
{
    uint32_t offset = 0;
    m_stride = 0;
    for(auto& element : m_elements)
    {
        element.offset = offset;
        offset += element.size;
        m_stride += element.size;
    }
}
