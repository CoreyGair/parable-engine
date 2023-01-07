#pragma once

#include "DescriptorSets.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"


namespace Parable::Vulkan
{


DescriptorSets::DescriptorSets(Device& device, vk::DescriptorPool& descriptorPool, std::vector<vk::DescriptorSetLayout>& layouts)
    : m_device{device}, m_descriptor_pool{descriptorPool}
{
    vk::DescriptorSetAllocateInfo info(
        m_descriptor_pool,
        layouts
    );

    m_descriptor_sets = std::move((*m_device).allocateDescriptorSets(info));
}


}
