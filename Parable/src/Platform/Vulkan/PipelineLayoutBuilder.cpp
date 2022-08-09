#include "PipelineLayoutBuilder.h"

#include <vulkan/vulkan.h>
#include "Platform/Vulkan/VulkanExceptions.h"


namespace Parable::Vulkan
{


VkPipelineLayout PipelineLayoutBuilder::create_layout(VkDevice device)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = m_descriptor_layouts.size();
    pipelineLayoutInfo.pSetLayouts = m_descriptor_layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = m_push_const_ranges.size();
    pipelineLayoutInfo.pPushConstantRanges = m_push_const_ranges.data();

    VkPipelineLayout layout;
    VkResult res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout);
    if (res != VK_SUCCESS)
    {
        throw VulkanFailedCreateException("pipeline layout", res);
    }
    return layout;
}


}