#include "pch.h"
#include "texture.h"

#include "renderer.h"
#include "platform/opengl/gl_texture.h"

std::shared_ptr<engine::texture_2d> engine::texture_2d::create(const std::string& path, const bool& clamp)
{
    switch(renderer::api())  
    {  
        case e_api_type::none: CORE_ASSERT(false, "[texture_2d] e_renderer_api::none currently not supported!"); return nullptr;  
        case e_api_type::open_gl: return std::make_shared<gl_texture_2d>(path, clamp);  
    }  

    CORE_ASSERT(false, "[texture_2d] Unknown renderer api!");  
    return nullptr;  
}

std::shared_ptr<engine::texture_2d> engine::texture_2d::create(unsigned char r, unsigned char g, unsigned char b)
{
	switch (renderer::api())
	{
	case e_api_type::none: CORE_ASSERT(false, "[texture_2d] e_renderer_api::none currently not supported!"); return nullptr;
	case e_api_type::open_gl: return std::make_shared<gl_texture_2d>(r, g, b);
	}

	CORE_ASSERT(false, "[texture_2d] Unknown renderer api!");
	return nullptr;
}
