#pragma once
#include "engine/renderer/mesh.h"

namespace engine
{
    /// \brief 
    class gl_mesh final : public mesh
    {
    public:
        gl_mesh(const std::vector<vertex>& vertices,
            const std::vector<uint32_t>& indices);

        const ref<vertex_array>& va() const override { return m_va; }
        uint32_t indices_count() const override { return (uint32_t)m_indices.size(); }
		std::vector<uint32_t> indices() const override { return m_indices; }
		std::vector<vertex> vertices() const override { return m_vertices; }

    private:
        std::vector<vertex>   m_vertices;
        std::vector<uint32_t> m_indices;

        ref<vertex_array> m_va{};
    };
}
