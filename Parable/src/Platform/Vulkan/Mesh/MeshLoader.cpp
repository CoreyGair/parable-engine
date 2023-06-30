#include "MeshLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Asset/AssetLoadInfo.h"

#include "../Wrapper/BufferSuballocator.h"
#include "../Wrapper/Buffer.h"
#include "../Wrapper/Vertex.h"

namespace Parable::Vulkan
{


MeshLoader::MeshLoader(Device device, PhysicalDevice physical_device, vk::Queue transfer_queue, uint32_t transfer_family)
    : m_device(device),
    m_physical_device(physical_device),
    m_transfer_queue(transfer_queue)
{
    vk::CommandPoolCreateInfo cmd_pool_info(   
        vk::CommandPoolCreateFlagBits::eTransient,
        transfer_family
    );
    m_command_pool = CommandPool(m_device, cmd_pool_info);

    vk::FenceCreateInfo fenceInfo {};
    m_transfer_finished_fence = m_device->createFence(fenceInfo);
}

// TODO: IDEA HERE FOR BATCHED LOADING
//      - in ctor, set up one largeish staging buffer each for vertex and index
//      - load(...) copies the mesh data to staging buffer, appending after previous loads
//          - must also keep track of each mesh in a list of "BufferedWrite" objects (vert_start,vert_size,vert_dest_start,ind_...)
//      - if load(...) call runs out of space in staging buffer, flush the staging and push the current call to a "QueuedWrite" queue
//          - QueuedWrite contains either:
//              - a loaded vertex and index array, plus destination info (to handle the first load() which runs out of space, where we have to load the model to know the size)
//              - MeshLoadInfo& plus destination info (to handle subsequent load()s after the first but before the staging is flushed, these still need to have vertices loaded to mem)
//              - [this might be easier to split into a "LoadedQueuedWrite" single head var + a "UnloadedQueuedWrite" (need better names) queue for the rest]
//          - "flushing" the staging buffer involves building the transfer commands to buffer and submitting
//              - flush is complete when the transfers are all done, can use a fence
//              - probs: have the fence waited on in a separate thread, which sets an atomic bool when flush completes?

Mesh MeshLoader::load(const MeshLoadInfo& load_info, BufferSuballocator& vertex_buffer_allocator, BufferSuballocator& index_buffer_allocator)
{
    const std::string& obj_path = load_info.get_obj_path();

    // load an obj model
    // will throw a runtime error if file badly formatted
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path.c_str())) {
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

    // TODO: optimise by choosing to use 16-bit indices if possible (vertex count low enough)

    vk::DeviceSize vertices_size = sizeof(vertices[0]) * vertices.size();
    vk::DeviceSize indices_size = sizeof(indices[0]) * indices.size();

    // allocate slices for vertex and index data
    BufferSlice vertex_slice = vertex_buffer_allocator.allocate(vertices_size);
    if (vertex_slice.size == 0) 
    {
        throw std::runtime_error("Out of vertex buffer space!");
    }
    BufferSlice index_slice = index_buffer_allocator.allocate(indices_size);
    if (index_slice.size == 0) 
    {
        throw std::runtime_error("Out of index buffer space!");
    }

    // copy data to staging buffers
    // TODO: in future can pre-allocate staging buffers and keep around
    //      prealloc some sensible size, and grow if need to load larger model
    //          (or could use multiple transfer commands??)
    BufferBuilder vertex_staging_buffer_builder;
    vertex_staging_buffer_builder.buffer_info.size = vertices_size;
    vertex_staging_buffer_builder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    vertex_staging_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertex_staging_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer vertex_staging_buffer = vertex_staging_buffer_builder.create(m_device, m_physical_device);
    vertex_staging_buffer.write((void*)vertices.data(), 0, vertices_size);

    BufferBuilder index_staging_buffer_builder;
    index_staging_buffer_builder.buffer_info.size = indices_size;
    index_staging_buffer_builder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    index_staging_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    index_staging_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer index_staging_buffer = index_staging_buffer_builder.create(m_device, m_physical_device);
    index_staging_buffer.write((void*)indices.data(), 0, indices_size);

    // transfer to main buffers
    CommandPool::SingleTimeCommandBuffer cmd_buffer = m_command_pool.begin_single_time_command();

    vertex_buffer_allocator.get_buffer().copy_from(vertex_staging_buffer, 0, vertex_slice.start, vertex_slice.size, cmd_buffer);
    index_buffer_allocator.get_buffer().copy_from(index_staging_buffer, 0, index_slice.start, index_slice.size, cmd_buffer);

    cmd_buffer.end_and_submit(m_transfer_queue, m_transfer_finished_fence);

    vk::Result res = m_device->waitForFences(1, &m_transfer_finished_fence, VK_TRUE, UINT64_MAX);
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for transfer fence.");
    }
    res = m_device->resetFences(1, &m_transfer_finished_fence);
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("failed to reset transfer fence.");
    }

    vertex_staging_buffer.destroy();
    index_staging_buffer.destroy();

    return Mesh(vertex_slice, index_slice);
}


}
