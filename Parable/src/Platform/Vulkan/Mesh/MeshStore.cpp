#include "MeshStore.h"

#include "pblpch.h"

#include "MeshLoader.h"

#include "Asset/AssetRegistry.h"
#include "Asset/AssetLoadInfo.h"

namespace Parable::Vulkan
{


Parable::MeshHandle MeshStore::load(AssetDescriptor descriptor)
{
    auto it = m_state_blocks.lower_bound(descriptor);
    if (it != m_state_blocks.end() && it->first == descriptor)
    {
        return MeshHandle(&it->second);
    }

    const Parable::MeshLoadInfo& load_info = AssetRegistry::resolve<Parable::MeshLoadInfo>(descriptor);

    Mesh new_mesh = m_mesh_loader->load(load_info, m_vertex_buffer_suballocator, m_index_buffer_suballocator);

    return MeshHandle();
}

Mesh& MeshStore::get_mesh(Parable::MeshHandle& handle)
{
    if (!handle)
    {
        throw std::runtime_error("get_mesh: null handle dereference.");
    }

    MeshStateBlock* state_block = static_cast<MeshStateBlock*>(handle.get_state_block());

    return state_block->get_mesh();
}


}
