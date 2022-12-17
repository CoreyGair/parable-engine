#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.h>

namespace Parable::Vulkan
{


class GPU;
class DescriptorPool;

class DescriptorSets
{
public:
    DescriptorSets(GPU& gpu, DescriptorPool& descriptor_pool, std::vector<VkDescriptorSetLayout>& layout);
    ~DescriptorSets();

    VkDescriptorSet& operator[](int i) { return m_descriptor_sets[i]; }

    void update_descriptor_set(size_t descriptor_set_index, uint32_t binding, VkDescriptorBufferInfo& buffer_info);

    const std::vector<VkDescriptorSet>& get_descriptor_sets() { return m_descriptor_sets; }

private:
    GPU& m_gpu;

    // the pool from which these sets were allocated
    DescriptorPool& m_descriptor_pool;

    std::vector<VkDescriptorSet> m_descriptor_sets;
};


}