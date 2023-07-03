#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Mesh.h"
#include "MeshStateBlock.h"

#include "Renderer/Handles.h"

#include "../Wrapper/BufferSuballocator.h"

#include "Asset/AssetDescriptor.h"

namespace Parable::Vulkan
{


class Loader;

/**
 * Handles the storage and lookup of Vulkan Mesh resources.
 */
class MeshStore
{
private:
    Loader& m_loader;

    std::map<AssetDescriptor,MeshStateBlock> m_state_blocks;

    BufferSuballocator m_vertex_buffer_suballocator;
    BufferSuballocator m_index_buffer_suballocator;

public:
    MeshStore(Buffer vertex_buffer, Buffer index_buffer, Loader& loader)
        : m_vertex_buffer_suballocator(vertex_buffer),
        m_index_buffer_suballocator(index_buffer),
        m_loader(loader)
    {}

    Buffer& get_vertex_buffer() { return m_vertex_buffer_suballocator.get_buffer(); }
    Buffer& get_index_buffer() { return m_index_buffer_suballocator.get_buffer(); }

    Parable::MeshHandle load(AssetDescriptor descriptor);

    const Mesh& get_mesh(Parable::MeshHandle& handle);
};


}
