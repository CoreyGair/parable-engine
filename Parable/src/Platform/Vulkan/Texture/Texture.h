#pragma once

#include "../Wrapper/Image.h"

namespace Parable::Vulkan
{


class Texture
{
private:
    Image m_image;

public:
    Texture(Image&& image) : m_image(std::move(image)) {}

    Image& get_image() const { return m_image; }
};


}
