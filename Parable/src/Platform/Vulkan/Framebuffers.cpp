#include "Framebuffers.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"
#include "GPU.h"
#include "Renderpass.h"


namespace Parable::Vulkan
{


Framebuffers::Framebuffers(GPU& gpu, const std::vector<VkImageView>& image_views, Renderpass& renderpass, VkExtent2D extent)
: m_gpu(gpu)
{
    resize_framebuffers(image_views.size());

    create_framebuffers(image_views, renderpass, extent);
}

Framebuffers::~Framebuffers()
{
    destroy_framebuffers();

    resize_framebuffers(0);
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
void Framebuffers::recreate_framebuffers(const std::vector<VkImageView>& image_views, Renderpass& renderpass, VkExtent2D extent)
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
void Framebuffers::create_framebuffers(const std::vector<VkImageView>& image_views, Renderpass& renderpass, VkExtent2D extent)
{
    PBL_CORE_ASSERT(image_views.size() == m_framebuffers.size());

    for (size_t i = 0; i < image_views.size(); i++) {
        VkImageView attachments[] = {
            image_views[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass.get_renderpass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        VkResult res = vkCreateFramebuffer(m_gpu.device, &framebufferInfo, nullptr, &m_framebuffers[i].framebuffer);
        if (res != VK_SUCCESS) {
            throw VulkanFailedCreateException("frame buffer", res);
        }
    }
}

/**
 * Destroys the VkFramebuffers held in the array.
 */
void Framebuffers::destroy_framebuffers()
{
    for (auto f : m_framebuffers) {
        vkDestroyFramebuffer(m_gpu.device, f.framebuffer, nullptr);
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
            vkDestroySemaphore(m_gpu.device, m_framebuffers[i].image_available_sem, nullptr);
            vkDestroySemaphore(m_gpu.device, m_framebuffers[i].render_finish_sem, nullptr);
            vkDestroyFence(m_gpu.device, m_framebuffers[i].inflight_fence, nullptr);
        }
        m_framebuffers.resize(size);
    }
    else if (size > curr_buffer_count)
    {
        m_framebuffers.resize(size);

        // create the extra sync objects needed
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // create the fence in the signalled state, so we dont wait for it on first frame :)

        for(size_t i = curr_buffer_count; i < curr_buffer_count+size; ++i)
        {
            VkResult res = vkCreateSemaphore(m_gpu.device, &semaphoreInfo, nullptr, &m_framebuffers[i].image_available_sem);
            if (res != VK_SUCCESS) {
                throw VulkanFailedCreateException("semaphore", res);
            }

            res = vkCreateSemaphore(m_gpu.device, &semaphoreInfo, nullptr, &m_framebuffers[i].render_finish_sem);
            if (res != VK_SUCCESS) {
                throw VulkanFailedCreateException("semaphore", res);
            }

            res = vkCreateFence(m_gpu.device, &fenceInfo, nullptr, &m_framebuffers[i].inflight_fence);
            if (res != VK_SUCCESS) {
                throw VulkanFailedCreateException("fence", res);
            }
        }
    }
}


}