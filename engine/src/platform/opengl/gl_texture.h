#pragma once
#include "engine/renderer/texture.h"

namespace engine
{
    class gl_texture_2d : public texture_2d
    {
    public:
        gl_texture_2d(const std::string& path, const bool& clamp);
		gl_texture_2d(unsigned char r, unsigned char g, unsigned char b);
        ~gl_texture_2d();
        void bind(uint32_t slot = 0) const override;

        uint32_t width() const override { return m_width; }
        uint32_t height() const override { return m_height; }
        std::string path() const override { return m_path; }

		bool loaded() const override { return s_loaded; }

		private:
			std::string m_path;
			uint32_t m_width{ 0 };
			uint32_t m_height{ 0 };
			uint32_t m_id{ 0 };
		bool s_loaded = true;
	};
}
