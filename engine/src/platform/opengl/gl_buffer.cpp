#include "pch.h"
#include "gl_buffer.h"
#include <glad\glad.h>

//=================== vertex buffer =======================================

engine::gl_vertex_buffer::gl_vertex_buffer(const float* vertices, uint32_t size)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

engine::gl_vertex_buffer::~gl_vertex_buffer()
{
    glDeleteBuffers(1, &m_id);
}

void engine::gl_vertex_buffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void engine::gl_vertex_buffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void engine::gl_vertex_buffer::layout(const buffer_layout& layout)
{
    m_layout = layout;
}

const engine::buffer_layout& engine::gl_vertex_buffer::layout() const
{
    return m_layout;
}

//=================== index buffer ========================================

engine::gl_index_buffer::gl_index_buffer(const uint32_t* vertices, uint32_t count)
    : m_count(count)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), vertices, GL_STATIC_DRAW);
}

engine::gl_index_buffer::~gl_index_buffer()
{
    glDeleteBuffers(1, &m_id);
}

void engine::gl_index_buffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void engine::gl_index_buffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t engine::gl_index_buffer::count() const
{
    return m_count;
}
