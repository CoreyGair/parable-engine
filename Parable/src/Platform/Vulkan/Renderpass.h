#pragma once

#include <pblpch.h>

#include <vulkan/vulkan.hpp>


namespace Parable::Vulkan
{


class Device;
class Swapchain;

/**
 * Describes how the image format of renderpass attachments is changed on renderpass create.
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
    Renderpass() = default;
    Renderpass(
        Device& device,
        Swapchain& swapchain,
        std::vector<vk::AttachmentDescription>&& attachments,
        std::vector<AttachmentFormat>&& attachment_formats,
        std::vector<vk::AttachmentReference>&& attachment_refs, 
        std::vector<vk::SubpassDescription>&& subpasses, 
        std::vector<vk::SubpassDependency>&& subpass_dependencies
    );

    Renderpass(Renderpass&& other) = default;
    Renderpass& operator=(Renderpass&& other)
    {
        Device m_device = other.m_device;
        vk::RenderPass m_renderpass = std::move(other.m_renderpass);

        std::vector<vk::AttachmentDescription> m_attachments = std::move(other.m_attachments);
        std::vector<AttachmentFormat> m_attachment_formats = std::move(m_attachment_formats);
        std::vector<vk::AttachmentReference> m_attachment_refs = std::move(m_attachment_refs);
        std::vector<vk::SubpassDescription> m_subpasses = std::move(m_subpasses);
        std::vector<vk::SubpassDependency> m_subpass_deps = std::move(m_subpass_deps);

        return *this;
    }

    vk::RenderPass get_renderpass() const { return m_renderpass; }

    void recreate_renderpass(Swapchain& swapchain);

private:
    void destroy_renderpass();
    void create_renderpass(Swapchain& swapchain);

    /**
     * The Device which owns/was used to create this renderpass.
     */
    Device m_device;

    vk::RenderPass m_renderpass;

    // have to keep these around as we rebuild the renderpass when swapchain rebuilt
    std::vector<vk::AttachmentDescription> m_attachments;
    std::vector<AttachmentFormat> m_attachment_formats;
    std::vector<vk::AttachmentReference> m_attachment_refs;
    std::vector<vk::SubpassDescription> m_subpasses;
    std::vector<vk::SubpassDependency> m_subpass_deps;
};

class RenderpassBuilder
{
public:
    Renderpass create(Device& device, Swapchain& swapchain);

    const std::vector<vk::AttachmentDescription>& get_attachments() const { return m_attachments; }
    const std::vector<AttachmentFormat>& get_attachment_formats() const { return m_attachment_formats; }
    const std::vector<vk::AttachmentReference>& get_attachment_refs() const { return m_attachment_refs; }
    const std::vector<vk::SubpassDescription>& get_subpasses() const { return m_subpasses; }
    const std::vector<vk::SubpassDependency>& get_subpass_deps() const { return m_subpass_deps; }

    /**
     * Adds an attachment.
     * 
     * @param attachment The vk::AttachmentDescription for the attachment.
     * @param format Describe how the attachment image format changes when renderpass is recreated. Defaults to AttachmentFormat::SWAPCHAIN.
     * @return uint32_t The index into the attachments list.
     */
    uint32_t add_attachment(vk::AttachmentDescription&& attachment, AttachmentFormat format = AttachmentFormat::SWAPCHAIN) {
        m_attachments.push_back(attachment);
        m_attachment_formats.push_back(format);

        return m_attachments.size() - 1;
    }

    uint32_t add_attachment_ref(uint32_t attachment_index, vk::ImageLayout layout) {
        m_attachment_refs.emplace_back(attachment_index, layout);
        return m_attachment_refs.size() - 1;
    }

    uint32_t add_subpass(vk::SubpassDescription&& subpass) {
        m_subpasses.push_back(subpass);
        return m_subpasses.size()-1;
    }

    uint32_t add_subpass_dependency(vk::SubpassDependency&& dependency) {
        m_subpass_deps.push_back(dependency);
        return m_subpass_deps.size()-1;
    }

private:
    std::vector<vk::AttachmentDescription> m_attachments;
    std::vector<AttachmentFormat> m_attachment_formats;
    std::vector<vk::AttachmentReference> m_attachment_refs;
    std::vector<vk::SubpassDescription> m_subpasses;
    std::vector<vk::SubpassDependency> m_subpass_deps;
};


}