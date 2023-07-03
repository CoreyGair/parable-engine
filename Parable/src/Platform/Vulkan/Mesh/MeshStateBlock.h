#pragma once

#include "pblpch.h"

#include "Mesh.h"

#include "Asset/AssetState.h"

namespace Parable::Vulkan
{


class MeshStateBlock : public Parable::AssetStateBlock
{
private:
    Mesh m_mesh;

public:
    MeshStateBlock(Mesh mesh) : m_mesh(mesh) {}

    const Mesh& get_mesh() { return m_mesh; }
    void set_mesh(const Mesh& mesh) { m_mesh = mesh; }
};


}
