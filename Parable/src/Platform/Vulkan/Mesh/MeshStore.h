#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Mesh.h"

#include "../Wrapper/BufferSuballocator.h"
#include "../ResourceStore.h"

#include "Asset/AssetDescriptor.h"

namespace Parable
{
class Mesh;
}

namespace Parable::Vulkan
{


/**
 * Handles the storage and lookup of Vulkan Mesh resources.
 */
class MeshStore : public ResourceStore<Parable::Mesh>
{
private:
    BufferSuballocator m_vertex_buffer_suballocator;
    BufferSuballocator m_index_buffer_suballocator;

public:
    MeshStore(Buffer vertex_buffer, Buffer index_buffer, Loader& loader)
        : ResourceStore<Parable::Mesh>(loader),
        m_vertex_buffer_suballocator(vertex_buffer),
        m_index_buffer_suballocator(index_buffer)
    {}

    Buffer& get_vertex_buffer() { return m_vertex_buffer_suballocator.get_buffer(); }
    Buffer& get_index_buffer() { return m_index_buffer_suballocator.get_buffer(); }

    std::unique_ptr<LoadTask> create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<Parable::Mesh>& storage_block) override;
};


}
