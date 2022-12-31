#include "Framebuffers.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"
#include "Renderpass.h"


namespace Parable::Vulkan
{


Framebuffers::Framebuffers(Device& device, const std::vector<vk::ImageView>& image_views, Renderpass& renderpass, vk::Extent2D extent)
: m_device(device)
{
    resize_framebuffers(image_views.size());

    create_framebuffers(image_views, renderpass, extent);
}

/**
 * Recreates the vulkan framebuffers using some new image views.
 * 
 * Useful for when something changes about the views (e.g. window resize).
 * 
 * @param image_views 
 * @param renderpass 
 * @param extent 
 */
void Framebuffers::recreate_framebuffers(const std::vector<vk::ImageView>& image_views, Renderpass& renderpass, vk::Extent2D extent)
{
    destroy_framebuffers();

    resize_framebuffers(image_views.size());

    create_framebuffers(image_views, renderpass, extent);
}

/**
 * Creates the VkFramebuffers from a set of image views.
 * 
 * @param image_views The views to construct framebuffers from.
 * @param renderpass The renderpass the framebuffers are used for.
 * @param extent The size of the images views (and so the framebuffers).
 */
void Framebuffers::create_framebuffers(const std::vector<vk::ImageView>& image_views, Renderpass& renderpass, vk::Extent2D extent)
{
    PBL_CORE_ASSERT(image_views.size() == m_framebuffers.size());

    for (size_t i = 0; i < image_views.size(); i++) {
        vk::FramebufferCreateInfo framebufferInfo(
            {},
            renderpass,
            1, // attachment count
            &image_views[i], // pAttachments
            extent.width,
            extent.height,
            1 // layers
        );

        m_framebuffers[i].framebuffer = m_device.createFramebuffer(framebufferInfo);
    }
}

/**
 * Destroys the VkFramebuffers held in the array.
 */
void Framebuffers::destroy_framebuffers()
{
    for (auto f : m_framebuffers) {
        m_device.destroyFramebuffer(f.framebuffer);
    }
}

/**
 * Resizes the internal framebuffer array.
 * 
 * Creates/destroys extra objects associated with each buffer (e.g. semaphores, fences) as needed.
 * 
 * @param size The new size of the framebuffer array
 */
void Framebuffers::resize_framebuffers(size_t size)
{
    size_t curr_buffer_count = m_framebuffers.size();

    if (size == curr_buffer_count)
    {
        return;
    }

    if (size < curr_buffer_count)
    {
        // destroy extra sync objects
        for(size_t i = curr_buffer_count-1; i > curr_buffer_count-size-1; --i)
        {
            m_device.destroySemaphore(m_framebuffers[i].image_available_sem);
            m_device.destroySemaphore(m_framebuffers[i].render_finish_sem);
            m_device.destroyFence(m_framebuffers[i].inflight_fence);
        }
        m_framebuffers.resize(size);
    }
    else if (size > curr_buffer_count)
    {
        m_framebuffers.resize(size);

        // create the extra sync objects needed
        vk::SemaphoreCreateInfo semaphoreInfo;

        vk::FenceCreateInfo fenceInfo(
            vk::FenceCreateFlagBits::eSignaled // create the fence in the signalled state, so we dont wait for it on first frame :)
        );

        for(size_t i = curr_buffer_count; i < curr_buffer_count+size; ++i)
        {   
            m_framebuffers[i].image_available_sem = m_device.createSemaphore(semaphoreInfo);
            m_framebuffers[i].render_finish_sem = m_device.createSemaphore(semaphoreInfo);
            m_framebuffers[i].inflight_fence = m_device.createFence(fenceInfo);
        }
    }
}


}