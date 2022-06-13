#pragma once
#include "texture.h"
#include "vertex_array.h"

namespace engine
{
    class mesh
    {
    public:
        struct vertex
        {
            union
            {
                float data[8]{0};
                struct
                {
                    glm::vec3 position;
                    glm::vec3 normal;
                    glm::vec2 tex_coords;
                };
            };
            /// \brief Empty ctor.
            vertex() = default;
            /// \brief Initializer list constructor.
            vertex(std::initializer_list<float> components)
            {
                int i = 0;
                for(const float elem : components)
                {
                    data[i++] = elem;
                }
            }
            /// \brief Specific constructor to allow initialization list with vectors.
            vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
                : position(pos), normal(norm), tex_coords(tex)
            {
            }
        };

    public:
        virtual ~mesh() = default;

        virtual const ref<vertex_array>& va() const = 0;
        virtual uint32_t indices_count() const = 0;
		virtual std::vector<vertex> vertices() const = 0;
		virtual std::vector<uint32_t> indices() const = 0;

        static ref<mesh> create(
            const std::vector<vertex>& vertices,
            const std::vector<uint32_t>& indices);


    };
}
