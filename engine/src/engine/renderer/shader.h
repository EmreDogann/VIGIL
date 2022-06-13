#pragma once
#include "glm/mat4x4.hpp"

namespace engine
{
    /// \brief Shader (platform agnostic) interface. Implemented in platform/api_name folder (i.e. opengl)
    class shader
    {
    public:
        virtual ~shader() = default;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;
        virtual const std::string& name() const = 0;

        static ref<shader> create(const std::string& file_path);
        static ref<shader> create(const std::string& name, const std::string& vertex_source, const std::string& fragment_source);
    };

    class shader_library final
    {
    public:
        void add(const std::string& name, const ref<shader>& shader);
        void add(const ref<shader>& shader);
        ref<shader> load(const std::string& filepath);
        ref<shader> load(const std::string& name, const std::string& filepath);

        ref<shader> get(const std::string& name);

        bool exists(const std::string& name) const;
    private:
        std::unordered_map<std::string, ref<shader>> m_shaders;
    };
}
