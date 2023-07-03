#include "TextureData.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

#include "Core/Base.h"

namespace Parable::Vulkan
{


TextureData::~TextureData()
{
    if (m_pixels)
    {
        stbi_image_free(m_pixels);
    }
}


TextureData TextureData::from_png(const std::string& png_path)
{
    int tex_width, tex_height, tex_channels;
    stbi_uc* pixels = stbi_load(png_path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
    
    PBL_CORE_ASSERT_MSG(pixels,stbi_failure_reason());

    vk::DeviceSize image_size = tex_width * tex_height * 4;

    return TextureData(pixels, image_size, TextureData::TextureDimensions{static_cast<uint32_t>(tex_width),static_cast<uint32_t>(tex_height)});
}


}
