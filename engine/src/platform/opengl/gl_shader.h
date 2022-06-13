#pragma once
#include "engine/renderer/shader.h"

namespace engine
{
    /// \brief OpenGL shader implementation.
    class gl_shader final : public shader
    {
    public:
        gl_shader(const std::string& file_path);
        gl_shader(const std::string& name, const std::string& vertex_source, const std::string& fragment_source);
        ~gl_shader();

        void bind() const override;
        void unbind() const override;
        const std::string& name() const override;

        void set_uniform(const std::string& name, int32_t val); 
        void set_uniform(const std::string& name, float val); 
        void set_uniform(const std::string& name, const glm::vec2& vec); 
        void set_uniform(const std::string& name, const glm::vec3& vec); 
        void set_uniform(const std::string& name, const glm::vec4& vec); 
        void set_uniform(const std::string& name, const glm::mat4& mat); 

    private:
        void compile(const std::unordered_map<uint32_t, std::string>& sources);
        static std::string read_file(const std::string& file_path);
        static std::unordered_map<uint32_t, std::string> pre_process(const std::string& source);
        static std::string extract_name(const std::string& file_path);

    private:
        uint32_t m_program_id{0};
        std::string m_name{};
    };
}
