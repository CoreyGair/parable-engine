#pragma once

#include "Renderer/Mesh.h"

#include "../Wrapper/BufferSlice.h"

namespace Parable::Vulkan
{


class Mesh : public Parable::Mesh
{
private:
    BufferSlice m_vertex_slice;
    BufferSlice m_index_slice;

public:
    ~Mesh() {}

    Mesh(BufferSlice vertex_slice, BufferSlice index_slice)
        : m_vertex_slice(vertex_slice), m_index_slice(index_slice)
    {}

    BufferSlice get_vertex_slice() const { return m_vertex_slice; }
    BufferSlice get_index_slice() const { return m_index_slice; }

    uint32_t get_index_count() const { return m_index_slice.size / sizeof(uint32_t); }
};


}
