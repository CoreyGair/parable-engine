#include "EffectData.h"

#include <vulkan/vulkan.hpp>

#include "Asset/EffectLoadInfo.h"
#include "Asset/AssetRegistry.h"

#include "Util/ReadFile.h"

namespace Parable::Vulkan
{


vk::ShaderStageFlags shader_stage_to_vk(ShaderStage stage)
{
    switch (stage)
    {
    case ShaderStage::Vertex:
        return vk::ShaderStageFlagBits::eVertex;
        break;
    case ShaderStage::Fragment:
        return vk::ShaderStageFlagBits::eVertex;
        break;
    case ShaderStage::Compute:
        throw std::exception("compute shader stage not implemented")
        break;
    }
}

EffectData::EffectData(const EffectLoadInfo& load_info)
{
    m_passes.reserve(load_info.get_passes().size());

    for (auto& shader_pass : load_info.get_passes())
    {   
        std::vector<std::pair<vk::ShaderStageFlags,std::vector<char>>> shaders;
        shaders.reserve(shader_pass.get_shader_stages().size());
        for (auto& shader : shader_pass.get_shader_stages())
        {
            const ShaderLoadInfo& shader_info = AssetRegistry::resolve(shader.shader);

            std::vector<char> code = Util::read_file(shader_info.get_spv_path());
            vk::ShaderModuleCreateInfo shader_create_info(
                {},
                code.size(),
                reinterpret_cast<uint32_t*>(code.data())
            );

            shaders.emplace_back(shader_stage_to_vk(shader_pass),device.createShaderModule(shader_create_info)));
        }

        m_passes.emplace_back(std::move(pass_data))
    }
}


}