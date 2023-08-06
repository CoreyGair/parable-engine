#include "AssetRegistry.h"

#include "pblpch.h"

#include "Core/Base.h"

#include <fstream>
#include <exception>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>

#include "AssetLoadInfo.h"

namespace Parable
{


std::map<AssetDescriptor, std::unique_ptr<AssetLoadInfo>> AssetRegistry::descriptor_to_load_info;

void AssetRegistry::init()
{
    int num_load_infos = 0;

    // loading from a test registry file
    std::ifstream registry_file("D:\\parable-engine\\test_asset_registry.json");
    if (!registry_file.is_open())
    {
        throw std::exception("Failed to open registry file.");
    }

    rapidjson::IStreamWrapper registry_file_wrapped{registry_file};

    rapidjson::Document registry_document{};
    registry_document.ParseStream(registry_file_wrapped);

    if (registry_document.HasParseError())
    {
        PBL_CORE_ERROR("Registry parse error: offset {}, {}", registry_document.GetErrorOffset(), rapidjson::GetParseError_En(registry_document.GetParseError()));
    }

    if (!registry_document.IsArray())
    {
        throw std::exception("Registry json is not an array.");
    }

    const auto& registry_array = registry_document.GetArray();

    for (rapidjson::SizeType i = 0; i < registry_array.Size(); ++i)
    {
        descriptor_to_load_info.emplace(i, AssetLoadInfoFactory::create(registry_array[i]));

        ++num_load_infos;
    }

    PBL_CORE_TRACE("Parsed {} load info objects.", num_load_infos);
}


}
