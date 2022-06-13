#include "pch.h"
#include "gl_texture.h"

#include "stb_image.h"
#include "glad/glad.h"

engine::gl_texture_2d::gl_texture_2d(const std::string& path, const bool& clamp) : m_path(path) {
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = NULL;
	data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
	if (data == NULL)
		s_loaded = false;

	CORE_ASSERT(data, "[gl_texture_2d] Texture not loaded correctly");
	m_width = width;
	m_height = height;

	GLenum internal_format = 0, data_format = 0;
	if (channels == 3) {
		internal_format = GL_RGB8;
		data_format = GL_RGB;
	} else if (channels == 4) {
		internal_format = GL_RGBA8;
		data_format = GL_RGBA;
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	// allocating memory to gpu to store the texture data
	const int mipmap_levels = 4;
	glTexStorage2D(GL_TEXTURE_2D, mipmap_levels, internal_format, m_width, m_height);

	// set texture params 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	// upload texture to gpu 
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, data_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// freeing allocated image buffer
	stbi_image_free(data);
}

engine::gl_texture_2d::gl_texture_2d(unsigned char r, unsigned char g, unsigned char b) {
	unsigned char data[12]; // a 2x2 texture at 24 bits 

	// Store the data 
	for (int i = 0; i < 12; i += 3) {
		data[i] = r;
		data[i + 1] = g;
		data[i + 2] = b;
	}

	m_width = 2;
	m_height = 2;

	// Generate the OpenGL texture id 
	GLuint textureId;
	glGenTextures(1, &textureId);

	// Bind this texture to its id 
	glBindTexture(GL_TEXTURE_2D, textureId);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Use mipmapping filter 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Generate the texture 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	m_id = textureId;

}

engine::gl_texture_2d::~gl_texture_2d() {
	glDeleteTextures(1, &m_id);
}

void engine::gl_texture_2d::bind(uint32_t slot /*= 0*/) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
}
