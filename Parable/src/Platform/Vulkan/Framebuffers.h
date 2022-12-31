#pragma once

#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{


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
    Framebuffers(Device& device, const std::vector<vk::ImageView>& image_views, Renderpass& renderpass, vk::Extent2D extent);
    Framebuffers(Framebuffers&& other)
    {
        m_device = other.m_device;
        m_framebuffers = std::move(other.m_framebuffers);
    }

    ~Framebuffers();

    const FramebufferData& operator[](size_t i) const { return m_framebuffers[i]; }

    void recreate_framebuffers(const std::vector<vk::ImageView>& image_views, Renderpass& renderpass, VkExtent2D extent);

private:
    void create_framebuffers(const std::vector<vk::ImageView>& image_views, Renderpass& renderpass, vk::Extent2D extent);
    void destroy_framebuffers();

    void resize_framebuffers(size_t size);

    Device m_device;
    
    std::vector<FramebufferData> m_framebuffers;
};


}