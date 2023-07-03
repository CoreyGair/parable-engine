#pragma once

#include <vulkan/vulkan.hpp>

#include "../Wrapper/Image.h"

namespace Parable::Vulkan
{


class Texture
{
private:
    Image m_image;

    vk::ImageView m_image_view;
    vk::Sampler m_sampler;
    vk::DescriptorSet m_descriptor_set;

public:
    Texture() = default;
    Texture(Image&& image, vk::ImageView image_view, vk::Sampler sampler, vk::DescriptorSet descriptor_set)
        : m_image(std::move(image)),
        m_image_view(image_view),
        m_sampler(sampler),
        m_descriptor_set(descriptor_set)
    {}
    Texture(const Texture& other)
    {
        m_image = other.m_image;
        m_image_view = other.m_image_view;
        m_sampler = other.m_sampler;
        m_descriptor_set = other.m_descriptor_set;
    }

    Texture& operator=(Texture&& other)
    {
        m_image = std::move(other.m_image);
        m_image_view = other.m_image_view;
        m_sampler = other.m_sampler;
        m_descriptor_set = other.m_descriptor_set;

        other.m_image = Image();
        other.m_image_view = nullptr;
        other.m_sampler = nullptr;
        other.m_descriptor_set = nullptr;

        return *this;
    }

    Image& get_image() { return m_image; }
    vk::ImageView& get_image_view() { return m_image_view; }
    vk::Sampler& get_sampler() { return m_sampler; }
    const vk::DescriptorSet& get_descriptor_set() const { return m_descriptor_set; }
};


}
