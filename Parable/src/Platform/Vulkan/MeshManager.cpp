#include "MeshManager.h"

#include "pblpch.h"
#include "Core/Base.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Wrapper/Vertex.h"

#include "UniformBufferObjects.h"

namespace Parable::Vulkan
{


MeshHandle MeshManager::load_mesh(std::string path)
{
    // LOAD model
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error(warn + err);
    }

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
                    attrib.texcoords.size() > 0 ? attrib.texcoords[2 * index.vertex_index + 0] : 0,
                    attrib.texcoords.size() > 0 ? 1.0f - attrib.texcoords[2 * index.vertex_index + 1] : 0
                }
            };
            
            if (uniqueVertices.find(vertex) == uniqueVertices.end()) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    // CREATE VERTEX BUFFER
    // use staging buffer to copy verts into device-local mem (faster memory)
    vk::DeviceSize vertexListSize = sizeof(vertices[0]) * vertices.size();

    BufferBuilder vertexStagingBufBuilder;
    vertexStagingBufBuilder.buffer_info.size = vertexListSize;
    vertexStagingBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    vertexStagingBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertexStagingBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer vertexStagingBuffer = vertexStagingBufBuilder.create(m_device, m_physical_device);

    // write the verts into the staging buffer
    vertexStagingBuffer.write((void*)vertices.data(), 0, vertexListSize);

    BufferBuilder vertexBufBuilder;
    vertexBufBuilder.buffer_info.size = vertexListSize;
    vertexBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    vertexBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertexBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    Buffer vertBuf = vertexBufBuilder.create(m_device, m_physical_device);

    // CREATE INDEX BUFFER
    // use staging buffer to copy indices into device-local mem (faster memory)
    VkDeviceSize indexListSize = sizeof(indices[0]) * indices.size();

    BufferBuilder indexStagingBufBuilder;
    indexStagingBufBuilder.buffer_info.size = indexListSize;
    indexStagingBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    indexStagingBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    indexStagingBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer indexStagingBuffer = indexStagingBufBuilder.create(m_device, m_physical_device);

    // write the verts into the staging buffer
    indexStagingBuffer.write((void*)indices.data(), 0, indexListSize);

    BufferBuilder indexBufBuilder;
    indexBufBuilder.buffer_info.size = indexListSize;
    indexBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    indexBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    indexBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    Buffer indexBuf = indexBufBuilder.create(m_device, m_physical_device);

    // copy from staging into vertex and indices
    auto cmdBuffer = m_command_pool.begin_single_time_command();

    vertBuf.copy_from(vertexStagingBuffer, cmdBuffer);
    indexBuf.copy_from(indexStagingBuffer, cmdBuffer);

    cmdBuffer.end_and_submit(m_transfer_queue);

    vertexStagingBuffer.destroy();
    indexStagingBuffer.destroy();
    
    m_meshes.emplace_back(std::move(vertBuf), std::move(indexBuf), static_cast<uint32_t>(indices.size()));

    // atm, we dont use the chunk field
    // just put the index of the mesh
    return MeshHandle{m_meshes.size()-1};
}


}