#include "MaterialLoadInfo.h"

#include "pblpch.h"

#include "AssetDescriptor.h"

#include <rapidjson/document.h>

namespace Parable
{


MaterialLoadInfo::MaterialParametersInfo::MaterialParametersInfo(const rapidjson::Value& source)
{
    if (!source.IsObject())
    {
        throw std::exception("MaterialParametersInfo source is not an object.");
    }

    auto json_info = source.GetObject();

    if (!json_info.HasMember("texture_descriptors") || !json_info["texture_descriptors"].IsArray())
    {
        throw std::exception("MaterialParametersLoadInfo does not contain a texture_descriptors array.");
    }

    auto texture_descriptors = json_info["texture_descriptors"].GetArray();
    m_texture_descriptors.reserve(texture_descriptors.Size());

    for (rapidjson::SizeType i = 0; i < texture_descriptors.Size(); ++i)
    {
        if (!texture_descriptors[i].IsUint64())
        {
            throw std::exception("MaterialParametersLoadInfo texture_descriptors contains a non-uint64 entry.");
        }

        m_texture_descriptors.push_back(texture_descriptors[i].GetUint64());
    }

    if (!json_info.HasMember("float_values") || !json_info["float_values"].IsArray())
    {
        throw std::exception("MaterialParametersLoadInfo does not contain a float_values array.");
    }

    auto float_values = json_info["float_values"].GetArray();
    m_float_uniform_values.reserve(float_values.Size());

    for (rapidjson::SizeType i = 0; i < float_values.Size(); ++i)
    {
        if (!float_values[i].IsFloat())
        {
            throw std::exception("MaterialParametersLoadInfo float_values contains a non-uint64 entry.");
        }

        m_float_uniform_values.push_back(float_values[i].GetFloat());
    }
}

MaterialLoadInfo::MaterialLoadInfo(const rapidjson::Value& source)
{
    if (!source.IsObject())
    {
        throw std::exception("MaterialLoadInfo source is not an object.");
    }

    auto json_info = source.GetObject();

    if (!json_info.HasMember("effect") || !source["effect"].IsUint64())
    {
        throw std::exception("MaterialLoadInfo does not contain an effect uint descriptor.");
    }

    AssetDescriptor effect = json_info["effect"].GetUint64();

    if (!json_info.HasMember("parameters") || !json_info["parameters"].IsObject())
    {
        throw std::exception("MaterialLoadInfo does not contain a parameters object.");
    }

    m_parameters_info = MaterialParametersInfo(json_info["parameters"].GetObject());
}


}
