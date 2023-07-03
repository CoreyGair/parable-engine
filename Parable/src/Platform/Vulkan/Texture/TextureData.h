#pragma once

#include <stb_image.h>
#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{



class TextureData
{
private:
    stbi_uc* m_pixels;
    vk::DeviceSize m_pixels_size;

    struct TextureDimensions
    {
        uint32_t width;
        uint32_t height;
    };

    TextureDimensions m_dimensions;

public:
    TextureData(stbi_uc* pixels, vk::DeviceSize size, TextureDimensions dimensions)
        : m_pixels(pixels),
        m_pixels_size(size),
        m_dimensions(dimensions)
    {}

    ~TextureData();

    stbi_uc* get_pixels() { return m_pixels; }
    vk::DeviceSize get_pixels_size() { return m_pixels_size; }
    const TextureDimensions& get_dimensions() { return m_dimensions; }

    static TextureData from_png(const std::string& png_path);
};


}
