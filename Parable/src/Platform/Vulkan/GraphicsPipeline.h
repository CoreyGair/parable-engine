#pragma once

#include <vulkan/vulkan.hpp>
#include <Platform/Vulkan/Vertex.h>

#include "Device.h"

namespace Parable::Vulkan
{


class Renderpass;

struct PipelineCreateInfo
{
public:
    vk::PipelineVertexInputStateCreateInfo vertex_input_state;
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state;
    vk::Viewport viewport;
    vk::Rect2D scissor;
    vk::PipelineRasterizationStateCreateInfo rasterizer_state;
    vk::PipelineMultisampleStateCreateInfo multisampling_state;
    vk::PipelineColorBlendStateCreateInfo color_blend_state;

private:
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
    std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments;

    std::vector<vk::VertexInputBindingDescription> binding_descriptions;
    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;

    friend class GraphicsPipeline;
    friend class GraphicsPipelineBuilder;
};

class GraphicsPipeline
{
public:
    GraphicsPipeline(Device& device, vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass, PipelineCreateInfo& info);

    void destroy()
    {
        destroy_pipeline();
    }

    vk::Pipeline get_pipeline() const { return m_pipeline; }

    void recreate_pipeline(vk::PipelineLayout& layout, Renderpass& renderpass);

    PipelineCreateInfo info;
    
private:
    void create_pipeline(vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass);
    void destroy_pipeline();

    Device m_device;

    vk::Pipeline m_pipeline;

    std::vector<vk::PipelineShaderStageCreateInfo> m_shader_stages;

    uint32_t m_subpass_index;
};

class GraphicsPipelineBuilder
{
public:
    GraphicsPipelineBuilder();

    GraphicsPipeline create(Device& device, vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass);

    uint32_t add_shader_stage(vk::PipelineShaderStageCreateInfo stage)
    {
        shaderStageCreateInfos.push_back(stage);
        return shaderStageCreateInfos.size()-1;
    }

    uint32_t add_blend_attachment(vk::PipelineColorBlendAttachmentState&& attachment)
    {
        colorBlendAttachmentStates.push_back(attachment);
        return colorBlendAttachmentStates.size()-1;
    }

    uint32_t add_binding_description(vk::VertexInputBindingDescription&& binding_description)
    {
        vertexInputBindingDescriptions.push_back(binding_description);
        return vertexInputBindingDescriptions.size()-1;
    }

    uint32_t add_attachment_description(vk::VertexInputAttributeDescription&& attribute_description)
    {
        vertexInputAttributeDescriptions.push_back(attribute_description);
        return vertexInputAttributeDescriptions.size()-1;
    }

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo;
    vk::Viewport viewport;
    vk::Rect2D scissor;
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos;
    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates;

    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;

};


}