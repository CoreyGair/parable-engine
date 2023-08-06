#pragma once

#include "AssetDescriptor.h"

#include "AssetLoadInfo.h"

#include <rapidjson/fwd.h>

#include "Renderer/Effect.h"

namespace Parable
{


class EffectLoadInfo : public AssetLoadInfo
{
public:
    /**
     * Describes a binding for a specific type of parameter into the buffer object.
     */
    struct ParameterBufferObjectBinding
    {
        /**
         * The type of parameter this slot takes.
         */
        EffectParameterType type;
        /**
         * The byte offset from the start of the UBO at which to place the parameter value.
         * 
         * This should respect GLSL/SPIRV UBO member alignment.
         */
        size_t offset;
    };

    /**
     * Describes a collection of shaders & render state which form a single pass over mesh data.
     * 
     * Essentially an abstraction of a pipeline.
     */
    class EffectPassInfo
    {
    public:
        /**
         * Describes a single shader and how it is used in this pass.
         */
        struct ShaderStageInfo
        {
            AssetDescriptor shader;
            ShaderStage stage;
        };

        EffectPassInfo(const rapidjson::Value& source);

        const std::vector<ShaderStageInfo>& get_shader_stages() const { return m_shader_stages; }

    private:
        std::vector<ShaderStageInfo> m_shader_stages;
    };

private: 
    std::vector<ParameterBufferObjectBinding> m_parameter_bindings;

    std::vector<EffectPassInfo> m_effect_passes;

public:
    LOAD_INFO_ASSET_TYPE(Effect)

    EffectLoadInfo(const rapidjson::Value& source);

    const std::vector<ParameterBufferObjectBinding>& get_parameter_uniform_bindings() const { return m_parameter_bindings; }
    const std::vector<EffectPassInfo>& get_passes() const { return m_effect_passes; }
};


}
