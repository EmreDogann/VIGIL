#include "pch.h"

#include "glad/glad.h"
#include "engine/renderer/shader.h"
#include "platform/opengl/gl_shader.h"
#include "text_manager.h"

engine::text_manager::text_manager(const std::string font_name) {
	init(font_name);
}

engine::text_manager::~text_manager() {}

void engine::text_manager::init(const std::string font_name) {
	if (FT_Init_FreeType(&ft))
		LOG_CORE_ERROR("[text] Could not init FreeType Library");

	// Load Text Manager with the font at the default path with the specified name.
	std::string font_path = "assets/fonts/" + font_name;

	FT_Face face;
	if (FT_New_Face(ft, font_path.c_str(), 0, &face))
		LOG_CORE_ERROR("[text] Failed to load font '" + font_name + "'");
	else {
		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++) {
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				LOG_CORE_ERROR("[text] Failed to load Glyph");
				continue;
			}
			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			engine::character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				(uint32_t)face->glyph->advance.x
			};
			characters.insert(std::pair<GLchar, engine::character>(c, character));
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}

void engine::text_manager::render_text(const engine::ref<engine::shader> s, std::string text, float x, float y, float scale, glm::vec4 colour) {
	// Activate corresponding render state
	std::dynamic_pointer_cast<engine::gl_shader>(s)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(s)->set_uniform("textColor", colour);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		engine::character ch = characters[*c];

		// Origin. Bottom-left corner of character in screen coordinates.
		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		// Width & Height of character in screen coordinates.
		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0.0, 0.0},
			{xpos, ypos, 0.0, 1.0},
			{xpos + w, ypos, 1.0, 1.0},

			{xpos, ypos + h, 0.0, 0.0},
			{xpos + w, ypos, 1.0, 1.0},
			{xpos + w, ypos + h, 1.0, 0.0}
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.texture_id);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Same as original render_text function but this time will pass back the dimensions/bounds of the text to be rendered.
void engine::text_manager::render_text(const engine::ref<engine::shader> s, std::string text, float x, float y, float scale, glm::vec4 colour, glm::vec4& dimensions) {
	// Activate corresponding render state
	std::dynamic_pointer_cast<engine::gl_shader>(s)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(s)->set_uniform("textColor", colour);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		engine::character ch = characters[*c];

		// Origin. Bottom-left corner of character in screen coordinates.
		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		// Width & Height of character in screen coordinates.
		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		if (c == text.begin()) {
			dimensions.x = (float)xpos;
			dimensions.y = (float)ypos;
			dimensions.w = dimensions.y + (float)h;
		}
		if (c + 1 == text.end()) {
			dimensions.z = (float)xpos + (float)w;
		}

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0.0, 0.0},
			{xpos, ypos, 0.0, 1.0},
			{xpos + w, ypos, 1.0, 1.0},

			{xpos, ypos + h, 0.0, 0.0},
			{xpos + w, ypos, 1.0, 1.0},
			{xpos + w, ypos + h, 1.0, 0.0}
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.texture_id);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Calculate the bounds/dimensions of the supplied text as a given scale and return it.
void engine::text_manager::get_text_dimensions(std::string text, float scale, glm::vec4& dimensions) {
	float x = 0.0f;
	float y = 0.0f;

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		engine::character ch = characters[*c];

		// Origin. Bottom-left corner of character in screen coordinates.
		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		// Width & Height of character in screen coordinates.
		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		if (c == text.begin()) {
			dimensions.x = (float)xpos;
			dimensions.y = (float)ypos;
			dimensions.w = dimensions.y + (float)h;
		}
		if (c + 1 == text.end()) {
			dimensions.z = (float)xpos + (float)w;
		}
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

engine::ref<engine::text_manager> engine::text_manager::create(const std::string font_name) {
	return std::make_shared<engine::text_manager>(font_name);
}
