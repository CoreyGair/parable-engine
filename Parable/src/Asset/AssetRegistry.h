#pragma once

#include "pblpch.h"

#include "AssetDescriptor.h"
#include "AssetLoadInfo.h"

namespace Parable
{


/**
 * Provides static access to AssetLoadInfo objects, mapped to by AssetDescriptors.
 */
class AssetRegistry
{
private:
    static std::map<AssetDescriptor, std::unique_ptr<AssetLoadInfo>> descriptor_to_load_info;

public:
    static void init();

    template<IsAssetLoadInfo ConcreteLoadInfoType>
    static const ConcreteLoadInfoType& resolve(AssetDescriptor descriptor)
    {
        if (auto it = descriptor_to_load_info.find(descriptor); it != descriptor_to_load_info.end())
        {
            const AssetLoadInfo& info = *(it->second);
            if (info.get_asset_type() == ConcreteLoadInfoType::asset_type) {
                return static_cast<const ConcreteLoadInfoType&>(info);
            }

            throw std::runtime_error("Asset resolved from descriptor is of unexpected type!");
        }

        throw std::runtime_error("Asset for descriptor not found!");
    }
};


}
