#include "DescriptorSetLayout.h"

#include "GPU.h"
#include "VulkanExceptions.h"

namespace Parable::Vulkan
{


DescriptorSetLayout::DescriptorSetLayout(GPU& gpu, std::vector<VkDescriptorSetLayoutBinding>& layout_bindings)
    : m_gpu{gpu}
{
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = layout_bindings.size();
    layout_info.pBindings = layout_bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(m_gpu.device, &layout_info, nullptr, &m_descriptor_set_layout);
    
    if (result != VK_SUCCESS) {
        throw VulkanFailedCreateException("descriptor set layout", result);
    }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(m_gpu.device, m_descriptor_set_layout, nullptr);
}

UPtr<DescriptorSetLayout> DescriptorSetLayoutBuilder::create(GPU& gpu)
{
    return std::make_unique<DescriptorSetLayout>(gpu, m_layout_bindings);
}


}