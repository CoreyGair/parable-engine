#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

namespace Parable
{
class EffectLoadInfo;
}

namespace Parable::Vulkan
{


class EffectData
{
private:
    class EffectPassData
    {
    public:
        EffectPassData(std::vector<std::pair<vk::ShaderStageFlags,vk::ShaderModule>>&& shaders)
            : m_shaders(shaders)
        {}

        const std::vector<std::pair<vk::ShaderStageFlags,vk::ShaderModule>>& get_shaders() const { return m_shaders; }

    private:
        std::vector<std::pair<vk::ShaderStageFlags,vk::ShaderModule>> m_shaders;
    };

    std::vector<EffectPassData> m_passes;

public:
    EffectData(const EffectLoadInfo& load_info, vk::Device device);

    const std::vector<EffectPassData>& get_passes() const { return m_passes; }
};


}