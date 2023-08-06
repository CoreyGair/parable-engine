#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Renderer/Effect.h"

namespace Parable::Vulkan
{


struct EffectPass
{   
    EffectPassType m_type;

    vk::Pipeline pipeline;

    vk::PipelineLayout pipeline_layout;
    std::array<vk::DescriptorSetLayout,4> descriptor_set_layouts;
};


}