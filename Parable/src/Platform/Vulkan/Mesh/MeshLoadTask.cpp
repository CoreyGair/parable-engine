#include "MeshLoadTask.h"

#include <vulkan/vulkan.hpp>

#include "../Wrapper/BufferSlice.h"
#include "../Wrapper/Buffer.h"
#include "../Wrapper/BufferSuballocator.h"

#include "MeshData.h"
#include "MeshStateBlock.h"

#include "Asset/AssetLoadInfo.h"

namespace Parable::Vulkan
{


MeshLoadTask::MeshLoadTask(
    const MeshLoadInfo& load_info,
    MeshStateBlock& mesh_state,
    BufferSuballocator& vertex_target_suballocator,
    BufferSuballocator& index_target_suballocator
)
    : m_load_info(load_info),
    m_mesh_state(mesh_state),
    m_vertex_target_suballocator(vertex_target_suballocator),
    m_index_target_suballocator(index_target_suballocator)
{}

void MeshLoadTask::record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer)
{
    MeshData mesh_data = MeshData::from_obj(m_load_info.get_obj_path());

    // allocate ranges in the GPU buffers for the data
    // as we now know how big it is
    BufferSlice vertex_slice = m_vertex_target_suballocator.allocate(mesh_data.get_vertices_size());
    if (vertex_slice.size == 0) 
    {
        throw std::runtime_error("Out of vertex buffer space!");
    }
    BufferSlice index_slice = m_index_target_suballocator.allocate(mesh_data.get_indices_size());
    if (index_slice.size == 0) 
    {
        throw std::runtime_error("Out of index buffer space!");
    }
    
    // we are now able to set the Mesh information correctly
    m_mesh_state.set_mesh(Mesh(vertex_slice, index_slice));

    // copy data to staging buffers
    // TODO: in future can pre-allocate staging buffers and keep around
    //      prealloc some sensible size, and grow if need to load larger model
    //          (or could use multiple transfer commands for larger meshes??)
    BufferBuilder vertex_staging_buffer_builder;
    vertex_staging_buffer_builder.buffer_info.size = mesh_data.get_vertices_size();
    vertex_staging_buffer_builder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    vertex_staging_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertex_staging_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    m_vertex_staging_buffer = vertex_staging_buffer_builder.create(device, physical_device);
    m_vertex_staging_buffer.write((void*)mesh_data.get_vertices().data(), 0, mesh_data.get_vertices_size());

    BufferBuilder index_staging_buffer_builder;
    index_staging_buffer_builder.buffer_info.size = mesh_data.get_indices_size();
    index_staging_buffer_builder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    index_staging_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    index_staging_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    m_index_staging_buffer = index_staging_buffer_builder.create(device, physical_device);
    m_index_staging_buffer.write((void*)mesh_data.get_indices().data(), 0, mesh_data.get_indices_size());

    // record copy commands
    m_vertex_target_suballocator.get_buffer().copy_from(m_vertex_staging_buffer, 0, vertex_slice.start, vertex_slice.size, command_buffer);
    m_index_target_suballocator.get_buffer().copy_from(m_index_staging_buffer, 0, index_slice.start, index_slice.size, command_buffer);
}

void MeshLoadTask::on_load_complete()
{
    m_vertex_staging_buffer.destroy();
    m_index_staging_buffer.destroy();

    // update the state block to show the mesh is now loaded
    m_mesh_state.set_load_state(AssetLoadState::Loaded);
}


}
