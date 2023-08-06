#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>


namespace Parable::Vulkan
{


/**
 * MaterialPass represents a single pass over mesh data.
 * 
 * Essentially an abstraction of a Pipeline.
 */
class MaterialPass
{
public:


private:
    vk::PipelineLayout m_pipeline_layout;
    std::array<vk::DescriptorSetLayout, 4> m_descriptor_set_layouts;

    vk::Pipeline m_pipeline;
}

class MaterialPassBuilder
{
public:
    vk::PipelineLayout m_pipeline_layout;
    std::array<vk::DescriptorSetLayout, 4> m_descriptor_set_layouts;

    void add_shader_stage(vk::ShaderModule module, vk::ShaderStageFlagBits stage)
    {
        m_shader_stages.emplace_back({module,stage});
    }

private:
    struct ShaderStage
    {
        vk::ShaderModule shader_module;
		vk::ShaderStageFlagBits stage;
    };
    std::vector<ShaderStage> m_shader_stages;
}


}