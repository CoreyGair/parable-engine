#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Renderer/Effect.h"

namespace Parable::Vulkan
{


class EffectPass;

class Effect : public Parable::Effect
{    
public:
    Effect() = default;
    Effect(
        std::vector<EffectPass>&& effect_passes;
        std::vector<ParameterBufferObjectBinding>&& parameter_bindings;
    )
        : m_passes(effect_passes),
        m_parameter_bindings(parameter_bindings)
    {}

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

private:
    std::vector<EffectPass> m_passes;

    std::vector<ParameterBufferObjectBinding> m_parameter_bindings;
};


}
