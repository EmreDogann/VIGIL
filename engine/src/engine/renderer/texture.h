    #pragma once

namespace engine
{
    enum class e_color_channels;
    enum class e_texture_filters;
    enum class e_wrap;

    class texture
    {
    public:
        virtual ~texture() = default;

        virtual uint32_t width() const = 0;
        virtual uint32_t height() const = 0;
        virtual std::string path() const = 0;

        virtual void bind(uint32_t slot = 0) const = 0;

		virtual bool loaded() const = 0;
    };

    class texture_2d : public texture
    {
    public:
        static ref<texture_2d> create(const std::string& path, const bool& clamp);
		static ref<texture_2d> create(unsigned char r, unsigned char g, unsigned char b);
    };
}
