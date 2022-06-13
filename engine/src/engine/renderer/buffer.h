#pragma once
#include "engine/core.h"

namespace engine
{

    //=================== buffer element ======================================

    /// \brief Type used to match the shading language types, in order to 
    /// construct 
    enum class e_shader_data_type
    {
        none = 0, float1, float2, float3, float4, int1, int2, int3, int4, mat3, mat4, boolean
    };

    static uint32_t shader_data_size(e_shader_data_type type)
    {
        switch (type)
        {
        case e_shader_data_type::float1:    return 4;
        case e_shader_data_type::float2:    return 4 * 2;
        case e_shader_data_type::float3:    return 4 * 3;
        case e_shader_data_type::float4:    return 4 * 4;
        case e_shader_data_type::int1:      return 4;
        case e_shader_data_type::int2:      return 4 * 2;
        case e_shader_data_type::int3:      return 4 * 3;
        case e_shader_data_type::int4:      return 4 * 4;
        case e_shader_data_type::mat3:      return 4 * 3 * 3;
        case e_shader_data_type::mat4:      return 4 * 4 * 4;
        case e_shader_data_type::boolean:   return 1;
        }

        CORE_ASSERT(false, "[shader_data_size] Unknown shader_data_type!");
        return 0;
    }

    /// \brief
    struct buffer_element
    {
        std::string name;
        e_shader_data_type type;
        uint32_t size = 0;
        uint32_t offset = 0;
        bool normalised = false;

        buffer_element(e_shader_data_type type, const std::string& name, bool normalised = false )
            :name(name), type(type), size(shader_data_size(type)), normalised(normalised)
        {
        }

        uint32_t components_count() const
        {
            switch (type)
            {
                case e_shader_data_type::float2:    return 2;
                case e_shader_data_type::float3:    return 3;
                case e_shader_data_type::float4:    return 4;
                case e_shader_data_type::mat3:      return 3 * 3;
                case e_shader_data_type::mat4:      return 4 * 4;
            }

            CORE_ASSERT(false, "[buffer_element] Uknown type!")
            return 0;
        };
    };

    //=================== buffer layout =======================================

    /// \brief Calculates the layout the the graphics api needs based on the given elements.
    class buffer_layout
    {
    public:
        /// \brief Default ctor.
        buffer_layout() = default;
        /// \brief Initializer list constructor.
        buffer_layout(std::initializer_list<buffer_element>);
        /// \brief Elements getter
        const std::vector<buffer_element>& elements() const;
        /// \brief Stride getter
        uint32_t stride() const;

        // To iterate through a range-based for-loop [i.e. for( auto elem : elements)]
        /// \brief Implicit implementation of .begin()
        std::vector<buffer_element>::iterator begin() { return m_elements.begin(); }
        /// \brief Implicit implementation of .end()
        std::vector<buffer_element>::iterator end() { return m_elements.end(); }
        /// \brief Implicit implementation of .cbegin()
        std::vector<buffer_element>::const_iterator begin() const { return m_elements.begin(); }
        /// \brief Implicit implementation of .cend()
        std::vector<buffer_element>::const_iterator end() const { return m_elements.end(); }


    private:
        /// \brief 
        void calculate_offset_and_stride();

    private:
        std::vector<buffer_element> m_elements{};
        uint32_t m_stride{ 0 };
    };

    //=================== vertex buffer =======================================

    /// \brief Vertex buffer (platform agnostic) interface
    class vertex_buffer
    {
    public:
        virtual ~vertex_buffer() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        /// \brief Layout setter.
        virtual void layout(const buffer_layout& layout) = 0;
        /// \brief Layout getter.
        virtual const buffer_layout& layout() const = 0;

        static ref<vertex_buffer> create(const float* vertices, uint32_t size);
    };

    //=================== index buffer ========================================

    /// \brief Index buffer (platform agnostic) interface
    class index_buffer
    {
    public:
        virtual ~index_buffer() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        /// \brief Count getter.
        virtual uint32_t count() const = 0;

        static ref<index_buffer> create(const uint32_t* indices, uint32_t count);
    };

}
