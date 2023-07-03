#include "MeshStore.h"

#include "pblpch.h"

#include "../Loader/Loader.h"

#include "MeshData.h"
#include "MeshLoadTask.h"

#include "Asset/AssetRegistry.h"
#include "Asset/AssetLoadInfo.h"

namespace Parable::Vulkan
{


Parable::MeshHandle MeshStore::load(AssetDescriptor descriptor)
{
    auto hint = m_state_blocks.lower_bound(descriptor);
    if (hint != m_state_blocks.end() && hint->first == descriptor)
    {
        return MeshHandle(&hint->second);
    }

    const Parable::MeshLoadInfo& load_info = AssetRegistry::resolve<Parable::MeshLoadInfo>(descriptor);
    
    // its state block goes into the Loading state until the mesh data is uploaded to GPU buffers
    auto it = m_state_blocks.emplace_hint(hint, descriptor, MeshStateBlock(Mesh()));
    it->second.set_load_state(AssetLoadState::Loading);

    // now we must submit a load task to copy the mesh data to gpu buffers
    std::unique_ptr<MeshLoadTask> load_task = std::make_unique<MeshLoadTask>(load_info, it->second, m_vertex_buffer_suballocator, m_index_buffer_suballocator);
    m_loader.submit_task(std::move(load_task));

    return MeshHandle(&it->second);
}

const Mesh& MeshStore::get_mesh(Parable::MeshHandle& handle)
{
    if (!handle)
    {
        throw std::runtime_error("get_mesh: null handle dereference.");
    }

    MeshStateBlock* state_block = static_cast<MeshStateBlock*>(handle.get_state_block());

    return state_block->get_mesh();
}


}
