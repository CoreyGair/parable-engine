#pragma once

#include <pblpch.h>

#include <vulkan/vulkan.hpp>
#include <Platform/Vulkan/Vertex.h>

#include "Device.h"

namespace Parable::Vulkan
{


class Renderpass;

struct PipelineInfo
{
    std::optional<vk::PipelineVertexInputStateCreateInfo> vertexInputStateCreateInfo;
    std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssemblyStateCreateInfo;

    std::optional<vk::PipelineTessellationStateCreateInfo> tessellationStateCreateInfo;
    std::optional<vk::PipelineDepthStencilStateCreateInfo> depthStencilStateCreateInfo;
    
    vk::Viewport viewport;
    vk::Rect2D scissor;
    
    std::optional<vk::PipelineRasterizationStateCreateInfo> rasterizationStateCreateInfo;
    std::optional<vk::PipelineMultisampleStateCreateInfo> multisampleStateCreateInfo;
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;

    std::optional<vk::PipelineDynamicStateCreateInfo> dynamicStateCreateInfo;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos;
    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates;

    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
};

class GraphicsPipeline
{
public:
    GraphicsPipeline() = default;
    GraphicsPipeline(Device& device, vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass, PipelineInfo&& info);

    void destroy()
    {
        destroy_pipeline();
    }

    operator vk::Pipeline&() { return m_pipeline; }
    vk::Pipeline& operator*() { return m_pipeline; }

    void recreate_pipeline(vk::PipelineLayout& layout, Renderpass& renderpass);
    
private:
    void create_pipeline(vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass);
    void destroy_pipeline();

    Device m_device;

    vk::Pipeline m_pipeline;

    std::vector<vk::PipelineShaderStageCreateInfo> m_shader_stages;

    uint32_t m_subpass_index;

    PipelineInfo info;
};

class GraphicsPipelineBuilder
{
public:
    GraphicsPipelineBuilder();

    GraphicsPipeline create(Device& device, vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass);

    /**
     * Pushes a shader stage and returns its index.
     */
    uint32_t add_shader_stage(vk::PipelineShaderStageCreateInfo&& stage)
    {
        info.shaderStageCreateInfos.push_back(stage);
        return info.shaderStageCreateInfos.size()-1;
    }
    /**
     * Pushes a blend attachment and returns its index.
     */
    uint32_t add_blend_attachment(vk::PipelineColorBlendAttachmentState&& attachment)
    {
        info.colorBlendAttachmentStates.push_back(attachment);
        return info.colorBlendAttachmentStates.size()-1;
    }
    /**
     * Pushes a binding description and returns its index.
     */
    uint32_t add_binding_description(vk::VertexInputBindingDescription&& binding_description)
    {
        info.vertexInputBindingDescriptions.push_back(binding_description);
        return info.vertexInputBindingDescriptions.size()-1;
    }
    /**
     * Pushes an attachment description and returns its index.
     */
    uint32_t add_attachment_description(vk::VertexInputAttributeDescription&& attribute_description)
    {
        info.vertexInputAttributeDescriptions.push_back(attribute_description);
        return info.vertexInputAttributeDescriptions.size()-1;
    }

    PipelineInfo info;

};


}