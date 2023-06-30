#pragma once

#include "pblpch.h"

#include <concepts>

#include "AssetDescriptor.h"

namespace Parable
{


enum class AssetType
{
    None = 0,
    Mesh,
    Texture,
};

class AssetLoadInfo
{
public:
    virtual ~AssetLoadInfo() = default;

    virtual AssetType get_asset_type() const = 0;
};

template<class T>
concept IsAssetLoadInfo = std::derived_from<T,AssetLoadInfo>;

#define LOAD_INFO_ASSET_TYPE(x) = const static AssetType asset_type = x; \
                                    AssetType get_asset_type() const override { return x; }

class MeshLoadInfo : public AssetLoadInfo
{
private:
    std::string m_obj_path;

public:
    //LOAD_INFO_ASSET_TYPE(AssetType::Mesh)
    const static AssetType asset_type = AssetType::Mesh;
    AssetType get_asset_type() const override { return AssetType::Mesh; }

    MeshLoadInfo(std::string obj_path) : m_obj_path(obj_path) {}

    const std::string& get_obj_path() const { return m_obj_path; }
};

class TextureLoadInfo : public AssetLoadInfo
{
private:
    std::string m_png_path;

public:
    //LOAD_INFO_ASSET_TYPE(AssetType::Texture)
    const static AssetType asset_type = AssetType::Texture;
    AssetType get_asset_type() const override { return AssetType::Texture; }

    TextureLoadInfo(std::string png_path) : m_png_path(png_path) {}

    const std::string& get_png_path() const { return m_png_path; }
};


}