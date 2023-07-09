#include "MeshData.h"

#include "Core/Base.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "../Wrapper/Vertex.h"

namespace Parable::Vulkan
{


size_t MeshData::get_vertices_size() const
{
    return sizeof(m_vertices[0]) * m_vertices.size();
}

size_t MeshData::get_indices_size() const
{
    return sizeof(m_indices[0]) * m_indices.size();
}

MeshData MeshData::from_obj(const std::string& obj_path)
{
    // load an obj model
    // will throw a runtime error if file badly formatted
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path.c_str());

    PBL_CORE_ASSERT_MSG(success, "tinyobj failed: {} : {}", warn, err);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    // collect all the shapes to one vertices array
    // dedupe vertices
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{
                .pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                },
                // temp, take color as position
                .color = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                },
                // temp, only if tex coords present
                .texCoord = {
                    attrib.texcoords.size() > 0 ? attrib.texcoords[2 * index.texcoord_index  + 0] : 0,
                    attrib.texcoords.size() > 0 ? 1.0f - attrib.texcoords[2 * index.texcoord_index  + 1] : 0
                }
            };
            
            if (uniqueVertices.find(vertex) == uniqueVertices.end()) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    return MeshData(std::move(vertices), std::move(indices));
}


}
