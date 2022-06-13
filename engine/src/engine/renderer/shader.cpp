#include "pch.h"
#include "shader.h"
#include "renderer.h"
#include "platform/opengl/gl_shader.h"

engine::ref<engine::shader> engine::shader::create(const std::string& file_path)
{
    switch (renderer::api())
    {
        case e_api_type::none: CORE_ASSERT(false, "[shader] e_renderer_api::none currently not supported!"); return nullptr;
        case e_api_type::open_gl: return std::make_shared<gl_shader>(file_path);
    }

    CORE_ASSERT(false, "[shader] Unknown renderer api!");
    return nullptr;
}

engine::ref<engine::shader> engine::shader::create(const std::string& name, const std::string& vertex_source, const std::string& fragment_source)
{
    CORE_ASSERT(!name.empty(), "[shader] Shader name cannot be empty");

    switch (renderer::api())
    {
        case e_api_type::none: CORE_ASSERT(false, "[shader] e_renderer_api::none currently not supported!"); return nullptr;
        case e_api_type::open_gl: return std::make_shared<gl_shader>(name, vertex_source, fragment_source);
    }

    CORE_ASSERT(false, "[shader] Unknown renderer api!");
    return nullptr;
}

//------------------------------------------------------------------------------------------------

void engine::shader_library::add(const std::string& name, const ref<shader>& shader)
{
    CORE_ASSERT(!exists(name), "[shader_library] shader already exists!");
    m_shaders[name] = shader;
}

void engine::shader_library::add(const ref<shader>& shader)
{
    auto& name = shader->name();
    add(name, shader);
}

engine::ref<engine::shader> engine::shader_library::load(const std::string& filepath)
{
    auto shader = shader::create(filepath);
    add(shader);
    return shader;
}

engine::ref<engine::shader> engine::shader_library::load(const std::string& name, const std::string& filepath)
{
    auto shader = shader::create(filepath);
    add(name, shader);
    return shader;
}

engine::ref<engine::shader> engine::shader_library::get(const std::string& name)
{
    CORE_ASSERT(exists(name), "[shader_library] shader not found!");
    return m_shaders[name];
}

bool engine::shader_library::exists(const std::string& name) const
{
    return m_shaders.find(name) != m_shaders.end();
}
