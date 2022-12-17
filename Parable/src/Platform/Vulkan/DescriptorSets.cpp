#pragma once

#include "DescriptorSets.h"

#include "Core/Base.h"

#include "GPU.h"

#include "VulkanExceptions.h"
#include "DescriptorPool.h"


namespace Parable::Vulkan
{


DescriptorSets::DescriptorSets(GPU& gpu, DescriptorPool& descriptor_pool, std::vector<VkDescriptorSetLayout>& layouts)
    : m_gpu{gpu}, m_descriptor_pool{descriptor_pool}
{
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = m_descriptor_pool.get_descriptor_pool();
    info.descriptorSetCount = layouts.size();
    info.pSetLayouts = layouts.data();

    m_descriptor_sets.resize(layouts.size());

    VkResult result = vkAllocateDescriptorSets(m_gpu.device, &info, m_descriptor_sets.data());

    if (result != VK_SUCCESS)
    {
        throw VulkanFailedCreateException("descriptor sets", result);
    } 
}

DescriptorSets::~DescriptorSets()
{
    vkFreeDescriptorSets(m_gpu.device, m_descriptor_pool.get_descriptor_pool(), m_descriptor_sets.size(), m_descriptor_sets.data());
}

void DescriptorSets::update_descriptor_set(size_t descriptor_set_index, uint32_t binding, VkDescriptorBufferInfo& buffer_info)
{
    PBL_CORE_ASSERT(descriptor_set_index >= 0 && descriptor_set_index < m_descriptor_sets.size());

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = m_descriptor_sets[descriptor_set_index];
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pImageInfo = nullptr; // Optional
    descriptor_write.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(m_gpu.device, 1, &descriptor_write, 0, nullptr);
}


}
