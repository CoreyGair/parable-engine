#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Asset/AssetDescriptor.h"

namespace Parable
{
class Effect;
}

namespace Parable::Vulkan
{


class Loader;

/**
 * Handles the storage and lookup of Vulkan Effect resources.
 */
class EffectStore : public ResourceStore<Parable::Effect>
{
public:
    EffectStore() {}

    std::unique_ptr<LoadTask> create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<Parable::Effect>& storage_block) override;
};


}
