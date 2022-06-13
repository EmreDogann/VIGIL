#pragma once
#include "buffer.h"

namespace engine
{
    class vertex_array
    {
    public:
        virtual ~vertex_array() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual void add_buffer(const ref<vertex_buffer>& vertexBuffer) = 0;
        virtual void add_buffer(const ref<index_buffer>& indexBuffer) = 0;

        virtual const std::vector<ref<vertex_buffer>>& vertex_buffers() const = 0;
        virtual const ref<index_buffer>& index_buffer() const = 0;

        static ref<vertex_array> create();
    };
}
