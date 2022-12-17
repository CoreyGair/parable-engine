#pragma once

#include "Core/Base.h"

#include <vulkan/vulkan.h>

namespace Parable::Vulkan
{

class GPU;

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(GPU& gpu, std::vector<VkDescriptorSetLayoutBinding>& layout_bindings);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout get_descriptor_set_layout()
    {
        return m_descriptor_set_layout;
    }

private:
    GPU& m_gpu;

    VkDescriptorSetLayout m_descriptor_set_layout;
};

class DescriptorSetLayoutBuilder
{
public:
    void add_descriptor_set_layout_binding(VkDescriptorSetLayoutBinding&& binding) 
    {
        m_layout_bindings.push_back(binding);
    }

    UPtr<DescriptorSetLayout> create(GPU& gpu);

private:
    std::vector<VkDescriptorSetLayoutBinding> m_layout_bindings;
};


}