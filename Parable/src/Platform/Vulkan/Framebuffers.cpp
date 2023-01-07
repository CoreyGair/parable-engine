#include "Framebuffers.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"
#include "Renderpass.h"


namespace Parable::Vulkan
{


Framebuffers::Framebuffers(Device& device, const std::vector<vk::ImageView>& imageViews, const std::vector<vk::ImageView>& extraAttachments, Renderpass& renderpass, vk::Extent2D extent)
    : m_device(device)
{
    create_framebuffers(imageViews, extraAttachments, renderpass, extent);
}

/**
 * Recreates the vulkan framebuffers using some new image views.
 * 
 * Useful for when something changes about the views (e.g. window resize).
 * 
 * @param imageViews 
 * @param renderpass 
 * @param extent 
 */
void Framebuffers::recreate_framebuffers(const std::vector<vk::ImageView>& imageViews, const std::vector<vk::ImageView>& extraAttachments, Renderpass& renderpass, vk::Extent2D extent)
{
    destroy_framebuffers();

    create_framebuffers(imageViews, extraAttachments, renderpass, extent);
}

/**
 * Creates the VkFramebuffers from a set of image views.
 * 
 * @param imageViews The views to construct framebuffers from.
 * @param renderpass The renderpass the framebuffers are used for.
 * @param extent The size of the images views (and so the framebuffers).
 */
void Framebuffers::create_framebuffers(const std::vector<vk::ImageView>& imageViews, const std::vector<vk::ImageView>& extraAttachments, Renderpass& renderpass, vk::Extent2D extent)
{
    m_framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        std::vector<vk::ImageView> attachments{imageViews[i]};
        attachments.reserve(1+extraAttachments.size());
        attachments.insert(attachments.end(), extraAttachments.begin(), extraAttachments.end());
    
        vk::FramebufferCreateInfo framebufferInfo(
            {},
            renderpass,
            attachments,
            extent.width,
            extent.height,
            1 // layers
        );

        m_framebuffers[i].framebuffer = (*m_device).createFramebuffer(framebufferInfo);


        vk::SemaphoreCreateInfo semaphoreInfo;
        vk::FenceCreateInfo fenceInfo(
            vk::FenceCreateFlagBits::eSignaled // create the fence in the signalled state, so we dont wait for it on first frame :)
        );
        m_framebuffers[i].image_available_sem = (*m_device).createSemaphore(semaphoreInfo);
        m_framebuffers[i].render_finish_sem = (*m_device).createSemaphore(semaphoreInfo);
        m_framebuffers[i].inflight_fence = (*m_device).createFence(fenceInfo);
    }
}

/**
 * Destroys the VkFramebuffers held in the array.
 */
void Framebuffers::destroy_framebuffers()
{
    for (auto f : m_framebuffers) {
        (*m_device).destroyFramebuffer(f.framebuffer);
        (*m_device).destroySemaphore(f.image_available_sem);
        (*m_device).destroySemaphore(f.render_finish_sem);
        (*m_device).destroyFence(f.inflight_fence);
    }
}


}