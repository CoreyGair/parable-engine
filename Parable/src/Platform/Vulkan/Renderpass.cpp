#include "Renderpass.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"

#include "GPU.h"
#include "Swapchain.h"


namespace Parable::Vulkan
{


Renderpass::Renderpass(
    GPU& gpu,
    Swapchain& swapchain,
    std::vector<VkAttachmentDescription>&& attachments,
    std::vector<AttachmentFormat>&& attachment_formats,
    std::vector<VkAttachmentReference>&& attachment_refs, 
    std::vector<VkSubpassDescription>&& subpasses, 
    std::vector<VkSubpassDependency>&& subpass_dependencies
) 
    : m_gpu(gpu),
    m_attachments(std::move(attachments)),
    m_attachment_formats(std::move(attachment_formats)),
    m_attachment_refs(std::move(attachment_refs)),
    m_subpasses(std::move(subpasses)),
    m_subpass_deps(std::move(subpass_dependencies))
{
    PBL_CORE_ASSERT(m_attachments.size() == m_attachment_formats.size());

    create_renderpass(swapchain);
}

void Renderpass::recreate_renderpass(const Swapchain& swapchain)
{
    destroy_renderpass();

    create_renderpass(swapchain);
}

void Renderpass::destroy_renderpass()
{
    vkDestroyRenderPass(m_gpu.device, m_renderpass, nullptr);
}

void Renderpass::create_renderpass(const Swapchain& swapchain)
{
    for (size_t i = 0; i < m_attachments.size(); ++i)
    {
        // update attachment formats
        if (m_attachment_formats[i] == AttachmentFormat::SWAPCHAIN)
        {
            m_attachments[i].format = swapchain.get_image_format();
        }
    }

    VkRenderPassCreateInfo renderpass_info{};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_info.attachmentCount = m_attachments.size();
    renderpass_info.pAttachments = m_attachments.data();
    renderpass_info.subpassCount = m_subpasses.size();
    renderpass_info.pSubpasses = m_subpasses.data();
    renderpass_info.dependencyCount = m_subpass_deps.size();
    renderpass_info.pDependencies = m_subpass_deps.data();

    VkResult res = vkCreateRenderPass(m_gpu.device, &renderpass_info, nullptr, &m_renderpass);
    if (res != VK_SUCCESS) {
        throw VulkanFailedCreateException("render pass", res);
    }
}

std::unique_ptr<Renderpass> RenderpassBuilder::create(GPU& gpu, Swapchain& swapchain)
{
    return std::make_unique<Renderpass>(
        gpu,
        swapchain,
        std::move(m_attachments),
        std::move(m_attachment_formats),
        std::move(m_attachment_refs),
        std::move(m_subpasses),
        std::move(m_subpass_deps)
    );
}


}