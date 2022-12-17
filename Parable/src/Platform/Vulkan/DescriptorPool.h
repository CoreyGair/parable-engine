#pragma once

#include <vulkan/vulkan.h>

namespace Parable::Vulkan
{

class GPU;
class DescriptorSets;

class DescriptorPool
{
public:
    DescriptorPool(GPU& gpu, VkDescriptorPoolCreateInfo& pool_info);
    ~DescriptorPool();

    std::unique_ptr<DescriptorSets> allocate_descriptor_sets(std::vector<VkDescriptorSetLayout>& layouts);

    VkDescriptorPool get_descriptor_pool() const { return m_descriptor_pool; }

private:
    GPU& m_gpu;

    VkDescriptorPool m_descriptor_pool;
};


}
