#include "Renderpass.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"

#include "Device.h"
#include "Swapchain.h"


namespace Parable::Vulkan
{


Renderpass::Renderpass(
    Device& device,
    Swapchain& swapchain,
    std::vector<vk::AttachmentDescription>&& attachments,
    std::vector<AttachmentFormat>&& attachment_formats,
    std::vector<vk::AttachmentReference>&& attachment_refs, 
    std::vector<vk::SubpassDescription>&& subpasses, 
    std::vector<vk::SubpassDependency>&& subpass_dependencies
)
    : m_device(device),
    m_attachments(std::move(attachments)),
    m_attachment_formats(std::move(attachment_formats)),
    m_attachment_refs(std::move(attachment_refs)),
    m_subpasses(std::move(subpasses)),
    m_subpass_deps(std::move(subpass_dependencies))
{
    PBL_CORE_ASSERT(m_attachments.size() == m_attachment_formats.size());

    create_renderpass(swapchain);
}

void Renderpass::recreate_renderpass(Swapchain& swapchain)
{
    destroy_renderpass();

    create_renderpass(swapchain);
}

void Renderpass::destroy_renderpass()
{
    (*m_device).destroyRenderPass(m_renderpass);
}

void Renderpass::create_renderpass(Swapchain& swapchain)
{
    auto format = m_attachment_formats.begin();
    for (auto attachment = m_attachments.begin(); attachment != m_attachments.end(); ++attachment, ++format)
    {
        // update attachment formats
        if (*format == AttachmentFormat::SWAPCHAIN)
        {
            attachment->format = swapchain.get_image_format();
        }
    }

    vk::RenderPassCreateInfo renderpass_info({}, m_attachments, m_subpasses, m_subpass_deps); 

    m_renderpass = (*m_device).createRenderPass(renderpass_info);
}

Renderpass RenderpassBuilder::create(Device& device, Swapchain& swapchain)
{
    return Renderpass(
        device,
        swapchain,
        std::move(m_attachments),
        std::move(m_attachment_formats),
        std::move(m_attachment_refs),
        std::move(m_subpasses),
        std::move(m_subpass_deps)
    );
}


}