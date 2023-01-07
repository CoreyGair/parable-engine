#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>
#include "Device.h"

namespace Parable::Vulkan
{


class DescriptorSets
{
public:
    DescriptorSets() = default;
    DescriptorSets(DescriptorSets&& other) = default;
    DescriptorSets(Device& device, vk::DescriptorPool& descriptorPool, std::vector<vk::DescriptorSetLayout>& layouts);

    DescriptorSets& operator=(DescriptorSets&& other)
    {
        m_device = other.m_device;
        m_descriptor_pool = other.m_descriptor_pool;
        m_descriptor_sets = std::move(other.m_descriptor_sets);

        return *this;
    }

    void destroy() 
    {
        (*m_device).freeDescriptorSets(m_descriptor_pool, m_descriptor_sets);
    }

    vk::DescriptorSet& operator[](int i) { return m_descriptor_sets[i]; }

    const std::vector<vk::DescriptorSet>& get_descriptor_sets() { return m_descriptor_sets; }

private:
    Device m_device;

    // the pool from which these sets were allocated
    vk::DescriptorPool m_descriptor_pool;

    std::vector<vk::DescriptorSet> m_descriptor_sets;
};


}