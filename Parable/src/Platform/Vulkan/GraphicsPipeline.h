#pragma once

#include <vulkan/vulkan.h>
#include <Platform/Vulkan/Vertex.h>

namespace Parable::Vulkan
{


class GPU;
class Renderpass;

struct PipelineCreateInfo
{
public:
    VkPipelineVertexInputStateCreateInfo vertex_input_state;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineRasterizationStateCreateInfo rasterizer_state;
    VkPipelineMultisampleStateCreateInfo multisampling_state;
    VkPipelineColorBlendStateCreateInfo color_blend_state;

private:
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;

    std::vector<VkVertexInputBindingDescription> binding_descriptions;
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions;

    friend class GraphicsPipeline;
    friend class GraphicsPipelineBuilder;
};

class GraphicsPipeline
{
public:
    GraphicsPipeline(GPU& gpu, VkPipelineLayout layout, Renderpass& renderpass, uint32_t subpass, PipelineCreateInfo& info);

    ~GraphicsPipeline()
    {
        destroy_pipeline();
    }

    VkPipeline get_pipeline() const { return m_pipeline; }

    void recreate_pipeline(VkPipelineLayout layout, Renderpass& renderpass);

    PipelineCreateInfo info;
    
private:
    void create_pipeline(VkPipelineLayout layout, Renderpass& renderpass, uint32_t subpass);
    void destroy_pipeline();

    GPU& m_gpu;

    VkPipeline m_pipeline;

    std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;

    uint32_t m_subpass_index;
};

class GraphicsPipelineBuilder
{
public:
    GraphicsPipelineBuilder(VkExtent2D initial_viewport_extent);

    std::unique_ptr<GraphicsPipeline> create(GPU& gpu, VkPipelineLayout layout, Renderpass& renderpass, uint32_t subpass);

    uint32_t add_shader_stage(VkPipelineShaderStageCreateInfo stage)
    {
        info.shader_stages.push_back(stage);
        return info.shader_stages.size()-1;
    }

    uint32_t add_blend_attachment(VkPipelineColorBlendAttachmentState&& attachment)
    {
        info.color_blend_attachments.push_back(attachment);
        return info.color_blend_attachments.size()-1;
    }

    uint32_t add_binding_description(VkVertexInputBindingDescription&& binding_description)
    {
        info.binding_descriptions.push_back(binding_description);
        return info.binding_descriptions.size()-1;
    }

    uint32_t add_attachment_description(VkVertexInputAttributeDescription&& attribute_description)
    {
        info.attribute_descriptions.push_back(attribute_description);
        return info.attribute_descriptions.size()-1;
    }

    PipelineCreateInfo info;

};


}