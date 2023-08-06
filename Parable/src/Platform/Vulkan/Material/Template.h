#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Pass.h"
#include "Parameters.h"

namespace Parable::Vulkan
{


enum class MaterialPassType 
{
    // example values for now
    Forward,
    DirectionalShadow,
    Transparent, 

    // end marker for pass type count
    _COUNT,
}

/**
 * MaterialTemplate collects together MaterialPasses to describe the complete rendering behaviour of a material.
 */
class MaterialTemplate
{
private:
    std::array<MaterialPass*,static_cast<size_t>(MaterialPassType::_COUNT)> m_passes;

    MaterialParameters m_default_parameters;
}


}