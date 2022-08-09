#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Parable::Vulkan
{


class PipelineLayoutBuilder
{
public:

    VkPipelineLayout create_layout(VkDevice device);

    void add_descriptor_layout(VkDescriptorSetLayout&& layout)
    {
        m_descriptor_layouts.push_back(layout);
    }
    void add_push_const_range(VkPushConstantRange&& range)
    {
        m_push_const_ranges.push_back(range);
    }

private:
    std::vector<VkDescriptorSetLayout> m_descriptor_layouts;
    std::vector<VkPushConstantRange> m_push_const_ranges;
};


}