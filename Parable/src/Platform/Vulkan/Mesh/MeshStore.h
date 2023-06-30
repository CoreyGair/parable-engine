#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Mesh.h"

#include "Renderer/Handles.h"

#include "../Wrapper/BufferSuballocator.h"

#include "Asset/AssetState.h"
#include "Asset/AssetDescriptor.h"

namespace Parable::Vulkan
{


class MeshStateBlock : public Parable::AssetStateBlock
{
private:
    Mesh m_mesh;

public:
    Mesh& get_mesh() { return m_mesh; }
};

class MeshLoader;

/**
 * Handles the storage and lookup of Vulkan Mesh resources.
 */
class MeshStore
{
private:
    UPtr<MeshLoader> m_mesh_loader;

    std::map<AssetDescriptor,MeshStateBlock> m_state_blocks;

    BufferSuballocator m_vertex_buffer_suballocator;
    BufferSuballocator m_index_buffer_suballocator;

public:
    MeshStore(Buffer vertex_buffer, Buffer index_buffer, UPtr<MeshLoader>&& mesh_loader)
        : m_vertex_buffer_suballocator(vertex_buffer),
        m_index_buffer_suballocator(index_buffer),
        m_mesh_loader(std::move(mesh_loader))
    {}

    Buffer& get_vertex_buffer() const { return m_vertex_buffer_suballocator.get_buffer(); }
    Buffer& get_index_buffer() const { return m_index_buffer_suballocator.get_buffer(); }

    Parable::MeshHandle load(AssetDescriptor descriptor);

    Mesh& get_mesh(Parable::MeshHandle& handle);
};


}
