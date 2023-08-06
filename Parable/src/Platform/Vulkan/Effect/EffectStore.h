#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Effect.h"

#include "Asset/AssetDescriptor.h"
#include "Asset/Handle.h"

namespace Parable
{
template<class ResourceType>
class ResourceStorageBlock;
}

namespace Parable::Vulkan
{


class Loader;

/**
 * Handles the storage and lookup of Vulkan Effect resources.
 */
class EffectStore
{
private:
    Loader& m_loader;

    std::map<AssetDescriptor,ResourceStorageBlock<Parable::Effect>> m_descriptor_effect_map;

public:
    EffectStore()

    Handle<Parable::Effect> load(AssetDescriptor descriptor);
};


}
