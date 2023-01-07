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
    // generate vertexInputStateCreateInfo based on binding descriptions
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
        {},
        info.vertexInputBindingDescriptions,
        info.vertexInputAttributeDescriptions
    );

    // attach all the state which could change between pipeline creations
    info.colorBlendStateCreateInfo.attachmentCount = info.colorBlendAttachmentStates.size();
    info.colorBlendStateCreateInfo.pAttachments = info.colorBlendAttachmentStates.data();
    
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo(
        {},
        1,
        &info.viewport,
        1,
        &info.scissor
    );
    
    vk::GraphicsPipelineCreateInfo pipelineInfo(
        {},
        info.shaderStageCreateInfos,
        &vertexInputStateCreateInfo,
        info.inputAssemblyStateCreateInfo ? &info.inputAssemblyStateCreateInfo.value() : nullptr,
        info.tessellationStateCreateInfo ? &info.tessellationStateCreateInfo.value() : nullptr,
        &viewportStateCreateInfo,
        info.rasterizationStateCreateInfo ? &info.rasterizationStateCreateInfo.value() : nullptr,
        info.multisampleStateCreateInfo ? &info.multisampleStateCreateInfo.value() : nullptr,
        info.depthStencilStateCreateInfo ? &info.depthStencilStateCreateInfo.value() : nullptr,
        &info.colorBlendStateCreateInfo,
        info.dynamicStateCreateInfo ? &info.dynamicStateCreateInfo.value() : nullptr, // dynamic state
        layout,
        renderpass,
        subpass,
        {}, // base pipeline handle
        {}  // base pipeline index
    );

    vk::ResultValue<std::vector<vk::Pipeline>> result = (*m_device).createGraphicsPipelines({}, {pipelineInfo});

    if (result.result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create pipeline");
    }

    m_pipeline = result.value[0];
}

void GraphicsPipeline::destroy_pipeline()
{
    (*m_device).destroyPipeline(m_pipeline);
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