#pragma once

#include "AssetDescriptor.h"

#include "AssetLoadInfo.h"

#include <rapidjson/fwd.h>

namespace Parable
{


class MaterialLoadInfo : public AssetLoadInfo
{
public:
    class MaterialParametersInfo
    {
    private:
        std::vector<AssetDescriptor> m_texture_descriptors;
        std::vector<float> m_float_uniform_values;

    public:
        MaterialParametersInfo() = default;
        MaterialParametersInfo(const rapidjson::Value& source);

        const std::vector<AssetDescriptor>& get_texture_descriptors() const { return m_texture_descriptors; }
        const std::vector<float>& get_float_uniform_values() const { return m_float_uniform_values; }
    };

private:
    AssetDescriptor m_effect_descriptor;

    MaterialParametersInfo m_parameters_info;

public:
    LOAD_INFO_ASSET_TYPE(Material)

    MaterialLoadInfo(const rapidjson::Value& source);

    AssetDescriptor get_effect_descriptor() const { return m_effect_descriptor; }
    const MaterialParametersInfo& get_material_parameters_info() const { return m_parameters_info; }
};


}
