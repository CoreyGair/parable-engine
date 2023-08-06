#pragma once

#include "pblpch.h"

#include <concepts>

#include "AssetDescriptor.h"

#include <rapidjson/fwd.h>

namespace Parable
{


enum class AssetType
{
    None = 0,
    Mesh,
    Texture,
    Shader,
    Effect,
    Material
};

class AssetLoadInfo
{
public:
    virtual ~AssetLoadInfo() = default;

    virtual AssetType get_asset_type() const = 0;
};

template<class T>
concept IsAssetLoadInfo = std::derived_from<T, AssetLoadInfo>;

#define LOAD_INFO_ASSET_TYPE(type) const static AssetType asset_type = AssetType::type;\
                                    AssetType get_asset_type() const override { return AssetType::type; }

class MeshLoadInfo : public AssetLoadInfo
{
private:
    std::string m_obj_path;

public:
    LOAD_INFO_ASSET_TYPE(Mesh)

    MeshLoadInfo(const rapidjson::Value& source);

    const std::string& get_obj_path() const { return m_obj_path; }
};

class TextureLoadInfo : public AssetLoadInfo
{
private:
    std::string m_png_path;

public:
    LOAD_INFO_ASSET_TYPE(Texture)

    TextureLoadInfo(const rapidjson::Value& source);

    const std::string& get_png_path() const { return m_png_path; }
};

class ShaderLoadInfo : public AssetLoadInfo
{
private:
    std::string m_spv_path;

public:
    LOAD_INFO_ASSET_TYPE(Shader)

    ShaderLoadInfo(const rapidjson::Value& source);

    const std::string& get_spv_path() const { return m_spv_path; }
};

class AssetLoadInfoFactory
{
public:
    AssetLoadInfoFactory() = delete;

    /**
     * Create a new AssetLoadInfo object from a source json document.
     * 
     * @param source The parse JSON document containing load info.
     * @return std::unique_ptr<AssetLoadInfo> 
     */
    static std::unique_ptr<AssetLoadInfo> create(const rapidjson::Value& source);
};


}