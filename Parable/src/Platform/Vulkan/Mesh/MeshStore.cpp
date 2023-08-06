#include "MeshStore.h"

#include "pblpch.h"

#include "../Loader/Loader.h"

#include "MeshData.h"
#include "MeshLoadTask.h"

#include "Asset/AssetRegistry.h"
#include "Asset/ResourceState.h"
#include "Asset/AssetLoadInfo.h"
#include "Asset/Handle.h"

namespace Parable::Vulkan
{


std::unique_ptr<LoadTask> MeshStore::create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<Parable::Mesh>& storage_block)
{
    const Parable::MeshLoadInfo& load_info = AssetRegistry::resolve<Parable::MeshLoadInfo>(descriptor);

    return std::make_unique<MeshLoadTask>(load_info, storage_block, m_vertex_buffer_suballocator, m_index_buffer_suballocator);
}


}
