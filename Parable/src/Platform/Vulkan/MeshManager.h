#pragma once

#include <vulkan/vulkan.hpp>

#include "Wrapper/Buffer.h"
#include "Wrapper/CommandPool.h"

#include "UniformBufferObjects.h"

#include "Render/Handles.h"

namespace Parable::Vulkan
{


class Mesh;


class MeshManager
{
public:
    MeshManager() = default;
    MeshManager(
        Device& device,
        PhysicalDevice& physicalDevice,
        CommandPool& commandPool,
        vk::Queue transferQueue
    )
        : m_device(device),
        m_physical_device(physicalDevice),
        m_command_pool(commandPool),
        m_transfer_queue(transferQueue)
    {}

    MeshHandle load_mesh(std::string path);

    Mesh& get_mesh(MeshHandle handle) { return m_meshes[handle.mesh]; }

private:
    // at the moment, doesnt own any of this
    // could change to using references to then just reference RendererVk's members
    // and then can be constructed by member initialisation list
    // (then again, this doesnt actually matter as these types aren't RAII, plus refs are just an extra indirection for potentially hot paths)
    Device m_device;
    PhysicalDevice m_physical_device;
    CommandPool m_command_pool;
    vk::Queue m_transfer_queue;

    std::vector<Mesh> m_meshes;
};

class Mesh
{
public:
    Mesh(Buffer&& vertexBuffer, Buffer&& indexBuffer, uint32_t indexCount) noexcept
        : m_vertex_buffer(vertexBuffer),
        m_index_buffer(indexBuffer),
        m_index_count(indexCount)
    {}
    Mesh(const Mesh& other) noexcept = default;
    void destroy()
    {
        m_vertex_buffer.destroy();
        m_index_buffer.destroy();
    }

    Buffer& get_vertex_buffer() { return m_vertex_buffer; }
    Buffer& get_index_buffer() { return m_index_buffer; }
    uint32_t get_index_count() const { return m_index_count; }


private:
    Buffer m_vertex_buffer;
    Buffer m_index_buffer;
    uint32_t m_index_count;

};



}
