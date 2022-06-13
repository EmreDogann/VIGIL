#pragma once
#include <glm/glm.hpp>
#include "engine/renderer/vertex_array.h"
#include "engine/renderer/mesh.h"

namespace engine
{  
    enum class e_api_type 
    { 
        none, open_gl = 1 
    };

    enum class e_primitive_type; 
    /// \brief   
    class renderer_api  
    {  
    public: 
        ~renderer_api() = default;
        static void init();  
        static void clear();  
        static void clear_color(const glm::vec4& color);  
        static void resize_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        
        static void enable_wireframe();
		static void enable_depth_mask();
		static void disable_depth_mask();
        static void disable_wireframe();  
        
        static void draw_indexed(const ref<vertex_array>& vertex_array);
		static void draw_indexed_lines(const ref<vertex_array>& vertex_array);
        static void draw_indexed(const ref<mesh>& mesh);  
        static void primitive_type(const e_primitive_type& type);  
        static e_api_type api() { return s_renderer_api; }

		static void line_width(float width);

    private:  
        static void enable_alpha();  
        static void enable_culling();

    private:
        static e_primitive_type s_primitive_type;
        static e_api_type       s_renderer_api; 
    };  
}
