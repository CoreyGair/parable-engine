#include "DescriptorPool.h"

#include "Core/Base.h"

#include "Platform/Vulkan/GPU.h"
#include "VulkanExceptions.h"

#include "DescriptorSets.h"

namespace Parable::Vulkan
{


DescriptorPool::DescriptorPool(GPU& gpu, VkDescriptorPoolCreateInfo& pool_info)
    : m_gpu{gpu}
{
    VkResult result = vkCreateDescriptorPool(m_gpu.device, &pool_info, nullptr, &m_descriptor_pool);

    if (result != VK_SUCCESS) {
        throw VulkanFailedCreateException("descriptor pool", result);
    }
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(m_gpu.device, m_descriptor_pool, nullptr);
}

std::unique_ptr<DescriptorSets> DescriptorPool::allocate_descriptor_sets(std::vector<VkDescriptorSetLayout>& layouts)
{
    return std::make_unique<DescriptorSets>(m_gpu, *this, layouts);
}


}
