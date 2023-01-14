#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace Parable::Vulkan
{


class Renderpass;

struct FramebufferData
{
    vk::Framebuffer framebuffer;
    vk::Semaphore image_available_sem;
    vk::Semaphore render_finish_sem;
    vk::Fence inflight_fence;
};

class Framebuffers
{
public:
    Framebuffers() = default;
    Framebuffers(Device& device, const std::vector<vk::ImageView>& imageviews, const std::vector<vk::ImageView>& extraAttachments, Renderpass& renderpass, vk::Extent2D extent);
    Framebuffers(Framebuffers&& other)
    {
        m_device = other.m_device;
        m_framebuffers = std::move(other.m_framebuffers);
    }

    Framebuffers& operator=(Framebuffers&& other)
    {
        m_device = other.m_device;
        m_framebuffers = std::move(other.m_framebuffers);
        return *this;
    }

    void destroy()
    {
        destroy_framebuffers();
    }

    const FramebufferData& operator[](size_t i) const { return m_framebuffers[i]; }

    void recreate_framebuffers(const std::vector<vk::ImageView>& imageviews, const std::vector<vk::ImageView>& extraAttachments, Renderpass& renderpass, vk::Extent2D extent);

private:
    void create_framebuffers(const std::vector<vk::ImageView>& imageviews, const std::vector<vk::ImageView>& extraAttachments, Renderpass& renderpass, vk::Extent2D extent);
    void destroy_framebuffers();

    Device m_device;
    
    std::vector<FramebufferData> m_framebuffers;
};


}