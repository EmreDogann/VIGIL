#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

namespace engine {
	class shader;


	struct character {
		uint32_t   texture_id; // ID handle of the glyph texture
		glm::ivec2 size;       // Size of glyph
		glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
		uint32_t   advance;    // Offset to advance to next glyph
	};

	class text_manager {
	public:
		text_manager(const std::string font_name);
		~text_manager();

		void                     render_text(engine::ref<engine::shader>, std::string text, float x, float y, float scale, glm::vec4 color);
		void                     render_text(engine::ref<engine::shader>, std::string text, float x, float y, float scale, glm::vec4 color, glm::vec4& dimensions);
		void                     get_text_dimensions(std::string text, float scale, glm::vec4& dimensions);
		static ref<text_manager> create(const std::string font_name);

	private:
		uint32_t                  VAO, VBO;
		FT_Library                ft;
		std::map<char, character> characters;
		std::string               font_name;
		void                      init(const std::string font_name);
	};
}
