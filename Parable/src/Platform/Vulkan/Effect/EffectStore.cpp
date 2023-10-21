#include "MeshStore.h"

#include "pblpch.h"

#include "EffectData.h"
#include "EffectLoadTask.h"

#include "Asset/AssetRegistry.h"
#include "Asset/EffectLoadInfo.h"

#include "Asset/ResourceState.h"
#include "Asset/Handle.h"

namespace Parable::Vulkan
{


std::unique_ptr<LoadTask> create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<Parable::Effect>& storage_block)
{
    const Parable::EffectLoadInfo& load_info = AssetRegistry::resolve<Parable::EffectLoadInfo>(descriptor);
}


}
