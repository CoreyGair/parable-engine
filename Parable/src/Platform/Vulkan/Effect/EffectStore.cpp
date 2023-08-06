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


Handle<Parable::Mesh> MeshStore::load(AssetDescriptor descriptor)
{
    auto hint = m_descriptor_mesh_map.lower_bound(descriptor);
    if (hint != m_descriptor_mesh_map.end() && hint->first == descriptor)
    {
        return Handle<Parable::Mesh>(hint->second);
    }

    const Parable::MeshLoadInfo& load_info = AssetRegistry::resolve<Parable::MeshLoadInfo>(descriptor);
    
    // its state block goes into the Loading state until the mesh data is uploaded to GPU buffers
    auto it = m_descriptor_mesh_map.emplace_hint(hint, descriptor, ResourceStorageBlock<Parable::Mesh>());
    it->second.set_load_state(ResourceLoadState::Loading);

    // now we must submit a load task to copy the mesh data to gpu buffers
    std::unique_ptr<MeshLoadTask> load_task = std::make_unique<MeshLoadTask>(load_info, it->second, m_vertex_buffer_suballocator, m_index_buffer_suballocator);
    m_loader.submit_task(std::move(load_task));

    return Handle<Parable::Mesh>(it->second);
}


}
