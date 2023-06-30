#pragma once

#include "Renderer/Handles.h"

#include "../Wrapper/Image.h"

namespace Parable::Vulkan
{


class Material
{
private:
    TextureHandle m_texture;

    vk::ImageView m_texture_view;
    vk::Sampler m_texture_sampler;
    
    vk::DescriptorSet m_descriptor_set;

public:
    Material(TextureHandle& texture, vk::DescriptorSet descriptor_set);
    ~Material();

};


}
