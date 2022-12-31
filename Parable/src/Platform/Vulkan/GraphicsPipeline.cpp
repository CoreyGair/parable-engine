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
    PipelineInfo&& info
) : m_device(device), info(info), m_subpass_index(subpass)
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
    info.colorBlendStateCreateInfo.attachmentCount = info.colorBlendAttachmentStates.size();
    info.colorBlendStateCreateInfo.pAttachments = info.colorBlendAttachmentStates.data();

    info.vertexInputStateCreateInfo.vertexBindingDescriptionCount = info.vertexInputBindingDescriptions.size();
    info.vertexInputStateCreateInfo.pVertexBindingDescriptions = info.vertexInputBindingDescriptions.data();

    info.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = info.vertexInputAttributeDescriptions.size();
    info.vertexInputStateCreateInfo.pVertexAttributeDescriptions = info.vertexInputAttributeDescriptions.data();

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo(
        {},
        1,
        &info.viewport,
        1,
        &info.scissor
    );
    
    vk::GraphicsPipelineCreateInfo pipelineInfo(
        info.shaderStageCreateInfos,
        info.vertexInputStateCreateInfo ? &info.vertexInputStateCreateInfo.value() : nullptr,
        info.inputAssemblyStateCreateInfo ? &info.inputAssemblyStateCreateInfo.value() : nullptr,
        info.tessellationStateCreateInfo ? &info.tessellationStateCreateInfo.value() : nullptr,
        &viewportStateCreateInfo,
        info.rasterizationStateCreateInfo ? &info.rasterizationStateCreateInfo.value() : nullptr,
        info.multisampleStateCreateInfo ? &info.multisampleStateCreateInfo.value() : nullptr,
        info.depthStencilStateCreateInfo ? &info.depthStencilStateCreateInfo.value() : nullptr,
        info.colorBlendStateCreateInfo ? &info.colorBlendStateCreateInfo.value() : nullptr,
        info.dynamicStateCreateInfo ? &info.dynamicStateCreateInfo.value() : nullptr, // dynamic state
        layout,
        renderpass,
        subpass,
        {}, // base pipeline handle
        {}  // base pipeline index
    );

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
    return GraphicsPipeline(device, layout, renderpass, subpass, std::move(info));
}


}