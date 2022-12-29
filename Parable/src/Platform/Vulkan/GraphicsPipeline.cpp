#include "GraphicsPipeline.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"
#include "Device.h"
#include "Renderpass.h"


namespace Parable::Vulkan
{


GraphicsPipeline::GraphicsPipeline(
    Device& device,
    vk::PipelineLayout& layout,
    Renderpass& renderpass,
    uint32_t subpass,
    PipelineCreateInfo& info
) : m_gpu(gpu), info(std::move(info)), m_subpass_index(subpass)
{
    create_pipeline(layout, renderpass, m_subpass_index);
}

void GraphicsPipeline::recreate_pipeline(vk::PipelineLayout& layout, Renderpass& renderpass)
{
    destroy_pipeline();

    create_pipeline(layout, renderpass, m_subpass_index);
}

void GraphicsPipeline::create_pipeline(vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass)
{
    // attach all the state which could change between pipeline creations
    info.color_blend_state.attachmentCount = info.color_blend_attachments.size();
    info.color_blend_state.pAttachments = info.color_blend_attachments.data();

    info.vertex_input_state.vertexBindingDescriptionCount = info.binding_descriptions.size();
    info.vertex_input_state.pVertexBindingDescriptions = info.binding_descriptions.data();

    info.vertex_input_state.vertexAttributeDescriptionCount = info.attribute_descriptions.size();
    info.vertex_input_state.pVertexAttributeDescriptions = info.attribute_descriptions.data();

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &info.viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &info.scissor;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // shader stages
    pipelineInfo.stageCount = info.shader_stages.size();
    pipelineInfo.pStages = info.shader_stages.data();
    // fixed function stages
    pipelineInfo.pVertexInputState = &info.vertex_input_state;
    pipelineInfo.pInputAssemblyState = &info.input_assembly_state;
    pipelineInfo.pViewportState = &viewport_state;
    pipelineInfo.pRasterizationState = &info.rasterizer_state;
    pipelineInfo.pMultisampleState = &info.multisampling_state;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &info.color_blend_state;
    pipelineInfo.pDynamicState = nullptr; // Optional
    // layout
    pipelineInfo.layout = layout;
    // render pass
    pipelineInfo.renderPass = renderpass.get_renderpass();
    pipelineInfo.subpass = subpass; // which subpass is this pipeline used on
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    m_pipeline = m_device.createGraphicsPipelines(std::vector<vk::GraphicsPipelineCreateInfo>{pipelineInfo});
}

void GraphicsPipeline::destroy_pipeline()
{
    vkDestroyPipeline(m_gpu.device, m_pipeline, nullptr);
}


// PIPELINE BUILDER

GraphicsPipelineBuilder::GraphicsPipelineBuilder()
{

}

GraphicsPipeline GraphicsPipelineBuilder::create(Device& device, vk::PipelineLayout& layout, Renderpass& renderpass, uint32_t subpass)
{
    PipelineCreateInfo new_info = std::move(info);

    return GraphicsPipeline(device, layout, renderpass, subpass, new_info);
}


}