#include "AssetLoadInfo.h"

#include "pblpch.h"

#include <rapidjson/document.h>

#include <exception>

#include "EffectLoadInfo.h"
#include "MaterialLoadInfo.h"

namespace Parable
{


MeshLoadInfo::MeshLoadInfo(const rapidjson::Value& source)
{
    auto object = source.GetObject();

    if (!object.HasMember("obj_path") || !object["obj_path"].IsString())
    {
        throw std::exception("MeshLoadInfo does not contain an obj_path string.");
    }

    m_obj_path = object["obj_path"].GetString();
}

TextureLoadInfo::TextureLoadInfo(const rapidjson::Value& source)
{
    auto object = source.GetObject();

    if (!object.HasMember("png_path") || !object["png_path"].IsString())
    {
        throw std::exception("TextureLoadInfo does not contain a png_path string.");
    }

    m_png_path = object["png_path"].GetString();
}

ShaderLoadInfo::ShaderLoadInfo(const rapidjson::Value& source)
{
    auto object = source.GetObject();

    if (!object.HasMember("spv_path") || !object["spv_path"].IsString())
    {
        throw std::exception("ShaderLoadInfo does not contain a spv_path string.");
    }

    m_spv_path = object["spv_path"].GetString();
}

std::unique_ptr<AssetLoadInfo> AssetLoadInfoFactory::create(const rapidjson::Value& source)
{
    if (!source.IsObject())
    {
        throw std::exception("AssetLoadInfo json is not an object.");
    }
    if (!source.HasMember("type") || !source["type"].IsString())
    {
        throw std::exception("AssetLoadInfo does not contain a type string.");
    }

    std::string_view type(source["type"].GetString());
    if (type == "mesh") return std::make_unique<MeshLoadInfo>(source);
    if (type == "texture") return std::make_unique<TextureLoadInfo>(source);
    if (type == "shader") return std::make_unique<ShaderLoadInfo>(source);
    if (type == "effect") return std::make_unique<EffectLoadInfo>(source);
    if (type == "material") return std::make_unique<MaterialLoadInfo>(source);

    throw std::exception("Unrecognised AssetLoadInfo type.");
}


}
