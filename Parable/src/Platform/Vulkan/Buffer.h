#pragma once

#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "PhysicalDevice.h"

namespace Parable::Vulkan
{


class Buffer
{
public:
    Buffer() = default;
    Buffer(Device& device, PhysicalDevice& physicalDevice, vk::BufferCreateInfo& info, vk::MemoryPropertyFlags requiredMemoryProperties);
    Buffer(Buffer&& other) noexcept
        : m_device(other.m_device),
        m_buffer(other.m_buffer),
        m_buffer_size(other.m_buffer_size),
        m_buffer_memory(other.m_buffer_memory),
        m_buffer_map(m_buffer_map)
    {}

    void destroy()
    {
        if (m_buffer_map) {
            unmap();
        }
        (*m_device).destroyBuffer(m_buffer);
        m_device.free_memory(m_buffer_memory);
    }

    Buffer& operator=(Buffer&& other)
    {
        m_device = other.m_device;

        m_buffer = other.m_buffer;
        m_buffer_size = other.m_buffer_size;
        m_buffer_memory = other.m_buffer_memory;

        m_buffer_map = other.m_buffer_map;

        return *this;
    }

    operator vk::Buffer&() { return m_buffer; }

    void write(void* data, vk::DeviceSize offset, vk::DeviceSize size);

    void map(vk::DeviceSize offset, vk::DeviceSize size);
    void unmap();

    vk::Buffer get_buffer() const { return m_buffer; }
    vk::DeviceSize get_size() const { return m_buffer_size; }
    void* get_map() const { return m_buffer_map; }

    void copy_from(Buffer& src, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size, vk::CommandBuffer transferCommandBuffer);
    void copy_from(Buffer& src, vk::CommandBuffer transferCommandBuffer);

    void copy_to(Buffer& dst, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size, vk::CommandBuffer transferCommandBuffer);
    void copy_to(Buffer& dst, vk::CommandBuffer transferCommandBuffer);

private:
    Device m_device;

    vk::Buffer m_buffer;
    vk::DeviceSize m_buffer_size;
    /**
     * The device-local memory backing this buffer.
     */
    vk::DeviceMemory m_buffer_memory;
    /**
     * Pointer to where this buffer is currently mapped (null if not mapped).
     */
    void* m_buffer_map = nullptr;
};

class BufferBuilder
{
public:
    Buffer create(Device& device, PhysicalDevice& physicalDevice);

    void emplace_back_vector(std::vector<Buffer>& vec, Device& device, PhysicalDevice& physicalDevice);

    vk::BufferCreateInfo buffer_info;
    vk::MemoryPropertyFlags required_memory_properties = {};
};


}