#pragma once

#include "pblpch.h"
#include "../Wrapper/Vertex.h"

namespace Parable::Vulkan
{


class MeshData
{
private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

public:
    MeshData() = default;
    MeshData(std::vector<Vertex>&& vertices, std::vector<uint32_t> indices) : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {}
    MeshData(MeshData&& other) : MeshData(std::move(other.m_vertices), std::move(other.m_indices)) {}

    MeshData& operator=(MeshData&& other)
    {
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
    }

    const std::vector<Vertex>& get_vertices() const { return m_vertices; };
    const std::vector<uint32_t>& get_indices() const { return m_indices; };

    size_t get_vertices_size() const;
    size_t get_indices_size() const;

    static MeshData from_obj(const std::string& obj_path);
};


}
