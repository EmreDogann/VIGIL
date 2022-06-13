#include "pch.h"
#include "gl_shader.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>

static uint32_t shader_type_from_string(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    CORE_ASSERT(false, "[gl_shader] Unknown shader type!");
    return 0;
}

static std::string shader_type_to_string(uint32_t type)
{
    if (type == GL_VERTEX_SHADER)
        return "vertex";
    if (type == GL_FRAGMENT_SHADER)
        return "fragment/pixel";

    CORE_ASSERT(false, "[gl_shader] Unknown shader type!");
    return 0;
}

engine::gl_shader::gl_shader(const std::string& file_path)
{
    const std::string source  = read_file(file_path);
    const auto shader_sources = pre_process(source);
    // Extract name from file_path
    m_name = extract_name(file_path);
    compile(shader_sources);
}

engine::gl_shader::gl_shader(const std::string& name, const std::string& vertex_source, const std::string& fragment_source)
    : m_name(name)
{
    std::unordered_map<uint32_t, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertex_source;
    sources[GL_FRAGMENT_SHADER] = fragment_source;
    compile(sources);
}

engine::gl_shader::~gl_shader()
{
    glDeleteProgram(m_program_id);
}


void engine::gl_shader::bind() const
{
    glUseProgram(m_program_id);
}

void engine::gl_shader::unbind() const
{
    glUseProgram(0);
}

void engine::gl_shader::set_uniform(const std::string& name, int32_t val) 
{ 
    const int32_t uniformLocation = glGetUniformLocation(m_program_id, name.c_str()); 
    glUniform1i(uniformLocation, val); 

    //LOG_CORE_TRACE("[shader] set_uniform (float) (prog {0}): uniform: '{1}' = {2}(float)", m_program_id, name, val); 
} 

void engine::gl_shader::set_uniform(const std::string& name, float val) 
{ 
    const int32_t uniformLocation = glGetUniformLocation(m_program_id, name.c_str()); 
    glUniform1f(uniformLocation, val); 

    //LOG_CORE_TRACE("[shader] set_uniform (float) (prog {0}): uniform: '{1}' = {2}(float)", m_program_id, name, val); 
} 

void engine::gl_shader::set_uniform(const std::string& name, const glm::vec2& vec) 
{
    const int32_t uniformLocation = glGetUniformLocation(m_program_id, name.c_str());
    glUniform2f(uniformLocation, vec.x, vec.y);

    //LOG_CORE_TRACE("[shader] set_uniform (float) (prog {0}): uniform: '{1}' = {2}(float)", m_program_id, name, vec); 
} 

void engine::gl_shader::set_uniform(const std::string& name, const glm::vec3& vec) 
{
    const int32_t uniformLocation = glGetUniformLocation(m_program_id, name.c_str());
    glUniform3f(uniformLocation, vec.x, vec.y, vec.z);

    //LOG_CORE_TRACE("[shader] set_uniform (float) (prog {0}): uniform: '{1}' = {2}(float)", m_program_id, name, vec); 
} 

void engine::gl_shader::set_uniform(const std::string& name, const glm::vec4& vec) 
{
    const int32_t uniformLocation = glGetUniformLocation(m_program_id, name.c_str());
    glUniform4f(uniformLocation, vec.x, vec.y, vec.z, vec.w);

    //LOG_CORE_TRACE("[shader] set_uniform (float) (prog {0}): uniform: '{1}' = {2}(float)", m_program_id, name, vec); 
} 

void engine::gl_shader::set_uniform(const std::string& name, const glm::mat4& mat) 
{
    const int32_t uniformLocation = glGetUniformLocation(m_program_id, name.c_str());
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat));

    //LOG_CORE_TRACE("[shader] set_uniform (glm::mat4) (prog {0}): uniform: '{1}' = {2}(mat4)", m_program_id, name, mat); 
}

const std::string& engine::gl_shader::name() const
{
    return m_name;
}

std::string engine::gl_shader::read_file(const std::string& file_path)
{
    std::string result;
    std::ifstream in(file_path, std::ios::in, std::ios::binary);
    if(in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    }
    else
    {
        LOG_CORE_ERROR("[gl_shader] Could not open file: {}", file_path);
    }
    return result;
}

std::unordered_map<uint32_t, std::string> engine::gl_shader::pre_process(const std::string& source)
{
    std::unordered_map<uint32_t, std::string> sources;
    const char* type_token = "#type";
    const size_t type_token_length = strlen(type_token);
    size_t pos = source.find(type_token, 0);
    while (pos != std::string::npos)
    {
        const size_t eol = source.find_first_of("\r\n", pos);
        CORE_ASSERT(eol != std::string::npos, "[gl_shader] Syntax error.");
        const size_t begin = pos + type_token_length + 1;
        std::string type = source.substr(begin, eol - begin);
        CORE_ASSERT(shader_type_from_string(type), "[gl_shader] Invalid shader type specified.");

        const size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(type_token, nextLinePos);
        sources[shader_type_from_string(type)] =
            source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }

    return sources;
}

void engine::gl_shader::compile(const std::unordered_map<uint32_t, std::string>& sources)
{
    const uint32_t program = glCreateProgram();
    std::vector<uint32_t> shader_ids(sources.size());

    for (auto&[type, source] : sources)
    {
        uint32_t shader = glCreateShader(type);
        std::string s_type = shader_type_to_string(type);

        const char* source_cstr = source.c_str();
        glShaderSource(shader, 1, &source_cstr, 0);

        LOG_CORE_INFO("[gl_shader] Compiling {}-shader: {}", s_type, m_name);
        glCompileShader(shader);

        int32_t is_compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            int32_t max_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

            std::vector<char> info_log(max_length);
            glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

            glDeleteShader(shader);

            LOG_CORE_ERROR("{0}", info_log.data());
            CORE_ASSERT(false, "[gl_shader] Compilation failed!");
            break;
        }

        glAttachShader(program, shader);
        shader_ids.push_back(shader);
    }

    m_program_id = program;

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    int32_t is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (is_linked == GL_FALSE)
    {
        int32_t max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<char> info_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);

        for (auto id : shader_ids)
            glDeleteShader(id);

        LOG_CORE_ERROR("{0}", info_log.data());
        CORE_ASSERT(false, "[gl_shader] Shader link failure!");
        return;
    }

    for (auto id : shader_ids)
        glDetachShader(program, id);
}

std::string engine::gl_shader::extract_name(const std::string& file_path)
{
    auto last_slash = file_path.find_last_of("/\\");
    last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
    const auto last_dot = file_path.rfind('.');
    const auto count = last_dot == std::string::npos ? file_path.size() - last_slash : last_dot - last_slash;
    return file_path.substr(last_slash, count);
} 

