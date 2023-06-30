#pragma once

#include "../Wrapper/BufferSlice.h"

namespace Parable::Vulkan
{


class Mesh
{
private:
    BufferSlice m_vertex_slice;
    BufferSlice m_index_slice;

public:
    Mesh(BufferSlice vertex_slice, BufferSlice index_slice)
        : m_vertex_slice(vertex_slice), m_index_slice(index_slice)
    {}

    BufferSlice get_vertex_slice() const { return m_vertex_slice; }
    BufferSlice get_index_slice() const { return m_index_slice; }
};


}
