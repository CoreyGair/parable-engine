#pragma once

#include <vulkan/vulkan.h>

namespace Parable::Vulkan
{


class GPU;
class Renderpass;

struct FramebufferData
{
    VkFramebuffer framebuffer;
    VkSemaphore image_available_sem;
    VkSemaphore render_finish_sem;
    VkFence inflight_fence;
};

class Framebuffers
{
public:
    Framebuffers(GPU& gpu, const std::vector<VkImageView>& image_views, Renderpass& renderpass, VkExtent2D extent);
    ~Framebuffers();

    const FramebufferData& operator[](size_t i) const { return m_framebuffers[i]; }

    void recreate_framebuffers(const std::vector<VkImageView>& image_views, Renderpass& renderpass, VkExtent2D extent);

private:
    void create_framebuffers(const std::vector<VkImageView>& image_views, Renderpass& renderpass, VkExtent2D extent);
    void destroy_framebuffers();

    void resize_framebuffers(size_t size);

    GPU& m_gpu;
    
    std::vector<FramebufferData> m_framebuffers;
};


}