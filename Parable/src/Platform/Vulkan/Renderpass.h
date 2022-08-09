#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace Parable::Vulkan
{


class GPU;
class Swapchain;

/**
 * Describes how the format of renderpass attachments is changed on renderpass create.
 */
enum class AttachmentFormat
{
    /**
     * The attachment format is left as-is.
     */
    CONSTANT,
    /**
     * The attachment format is updated to that of the swapchain.
     */
    SWAPCHAIN,
};

class Renderpass
{
public:
    Renderpass(
        GPU& gpu,
        Swapchain& swapchain,
        std::vector<VkAttachmentDescription>&& attachments,
        std::vector<AttachmentFormat>&& attachment_formats,
        std::vector<VkAttachmentReference>&& attachment_refs, 
        std::vector<VkSubpassDescription>&& subpasses, 
        std::vector<VkSubpassDependency>&& subpass_dependencies
    );

    Renderpass(Renderpass&& other) = default;

    VkRenderPass get_renderpass() const { return m_renderpass; }

    void recreate_renderpass(const Swapchain& swapchain);

private:
    void destroy_renderpass();
    void create_renderpass(const Swapchain& swapchain);

    /**
     * The GPU which owns/was used to create this renderpass.
     */
    GPU& m_gpu;

    VkRenderPass m_renderpass;

    // have to keep these around as we rebuild the renderpass when swapchain rebuilt
    std::vector<VkAttachmentDescription> m_attachments;
    std::vector<AttachmentFormat> m_attachment_formats;
    std::vector<VkAttachmentReference> m_attachment_refs;
    std::vector<VkSubpassDescription> m_subpasses;
    std::vector<VkSubpassDependency> m_subpass_deps;
};

class RenderpassBuilder
{
public:
    std::unique_ptr<Renderpass> create(GPU& gpu, Swapchain& swapchain);

    const std::vector<VkAttachmentDescription>& get_attachments() const { return m_attachments; }
    const std::vector<VkAttachmentReference>& get_attachment_refs() const { return m_attachment_refs; }
    const std::vector<VkSubpassDescription>& get_subpasses() const { return m_subpasses; }
    const std::vector<VkSubpassDependency>& get_subpass_deps() const { return m_subpass_deps; }

    uint32_t add_attachment(VkAttachmentDescription attachment, AttachmentFormat format = AttachmentFormat::SWAPCHAIN) {
        m_attachments.push_back(attachment);
        
        m_attachment_formats.push_back(format);
        return m_attachments.size() - 1;
    }

    uint32_t add_attachment_ref(uint32_t attachment_index, VkImageLayout layout) {
        m_attachment_refs.push_back({.attachment = attachment_index, .layout = layout});
        return m_attachment_refs.size() - 1;
    }

    uint32_t add_subpass(VkSubpassDescription subpass) {
        m_subpasses.push_back(subpass);
        return m_subpasses.size()-1;
    }

    uint32_t add_subpass_dependency(VkSubpassDependency dependency) {
        m_subpass_deps.push_back(dependency);
        return m_subpass_deps.size()-1;
    }

private:
    std::vector<VkAttachmentDescription> m_attachments;
    std::vector<AttachmentFormat> m_attachment_formats;
    std::vector<VkAttachmentReference> m_attachment_refs;
    std::vector<VkSubpassDescription> m_subpasses;
    std::vector<VkSubpassDependency> m_subpass_deps;
};


}