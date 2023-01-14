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
        vk::Queue transferQueue,
        vk::DescriptorPool drawDescriptorPool,
        vk::DescriptorSetLayout drawDescriptorSetLayout
    )
        : m_device(device),
        m_physical_device(physicalDevice),
        m_command_pool(commandPool),
        m_transfer_queue(transferQueue),
        m_draw_descriptor_pool(drawDescriptorPool),
        m_draw_descriptor_set_layout(drawDescriptorSetLayout)
    {}

    MeshHandle load_mesh(std::string path);

    void set_mesh_transform(MeshHandle handle, glm::mat4& transform);

    std::vector<Mesh>& get_meshes() { return m_meshes; }

private:
    // at the moment, doesnt own any of this
    // could change to using references to then just reference RendererVk's members
    // and then can be constructed by member initialisation list
    Device m_device;
    PhysicalDevice m_physical_device;
    CommandPool m_command_pool;
    vk::Queue m_transfer_queue;

    // TODO: extract draw descriptor set creation etc into a manager class
    // hopefully which supports a variable number of descriptors (atm fixed)
    vk::DescriptorPool m_draw_descriptor_pool;
    vk::DescriptorSetLayout m_draw_descriptor_set_layout;

    std::vector<Mesh> m_meshes;
};

class Mesh
{
public:
    Mesh(Buffer&& vertexBuffer, Buffer&& indexBuffer, uint32_t indexCount, vk::DescriptorSet drawDescriptorSet, Buffer&& transformBuffer) noexcept
        : m_vertex_buffer(vertexBuffer),
        m_index_buffer(indexBuffer),
        m_index_count(indexCount),
        m_draw_descriptor_set(drawDescriptorSet),
        m_transform_buffer(transformBuffer)
    {}
    Mesh(const Mesh& other) noexcept = default;
    void destroy()
    {
        m_vertex_buffer.destroy();
        m_index_buffer.destroy();
    }

    void set_transform(glm::mat4& mat)
    {
        PerDrawUniformBufferObject ubo{mat};
        m_transform_buffer.write(&ubo, 0, sizeof(ubo));
    }

    Buffer& get_vertex_buffer() { return m_vertex_buffer; }
    Buffer& get_index_buffer() { return m_index_buffer; }
    vk::DescriptorSet get_descriptor_set() const { return m_draw_descriptor_set; }
    uint32_t get_index_count() const { return m_index_count; }


private:
    Buffer m_vertex_buffer;
    Buffer m_index_buffer;
    uint32_t m_index_count;
    
    vk::DescriptorSet m_draw_descriptor_set;
    Buffer m_transform_buffer;
};



}
