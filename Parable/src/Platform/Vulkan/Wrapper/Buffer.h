#pragma once

#include "Core/Base.h"

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
    Buffer(const Buffer& other) noexcept = default;
    Buffer(Buffer&& other) noexcept = default;

    void destroy()
    {
        m_device->destroyBuffer(m_buffer);
        m_device.free_memory(m_buffer_memory);
    }

    Buffer& operator=(Buffer&& other)
    {
        m_device = other.m_device;

        m_buffer = other.m_buffer;
        m_buffer_size = other.m_buffer_size;
        m_buffer_memory = other.m_buffer_memory;

        return *this;
    }

    operator vk::Buffer&() { return m_buffer; }

    vk::Buffer get_buffer() const { return m_buffer; }
    void* get_map() const { return m_buffer_map; }
    vk::DeviceSize get_size() const { return m_buffer_size; }

    /**
     * Write some data into this buffer.
     * 
     * Assumes the buffer is host-coherent.
     * 
     * @param data The data to write.
     * @param offset The offset into the buffer to write at.
     * @param size The size of data to be written.
     */
    void write(void* data, vk::DeviceSize offset, vk::DeviceSize size)
    {
        PBL_CORE_ASSERT(size <= m_buffer_size);
        PBL_CORE_ASSERT(!m_buffer_map);

        void* bufferMap = m_device->mapMemory(m_buffer_memory, offset, size);
        memcpy(bufferMap, data, (size_t)size);
        m_device->unmapMemory(m_buffer_memory);
    }

    /**
     * Write some data into this buffer and flush the range.
     * 
     * Assumes the buffer is noy host-coherent.
     * 
     * @param data The data to write.
     * @param offset The offset into the buffer to write at.
     * @param size The size of data to be written.
     */
    void write_flush(void* data, vk::DeviceSize offset, vk::DeviceSize size)
    {
        PBL_CORE_ASSERT(size <= m_buffer_size);
        PBL_CORE_ASSERT(!m_buffer_map);

        void* bufferMap = m_device->mapMemory(m_buffer_memory, offset, size);
        memcpy(bufferMap, data, (size_t)size);
        m_device->flushMappedMemoryRanges({vk::MappedMemoryRange(m_buffer_memory, offset, size)});
        m_device->unmapMemory(m_buffer_memory);
    }

    /**
     * Maps this buffer to host memory.
     * 
     * This is intended for long term persistent maps, for one-off writes use Buffer::write instead.
     * 
     * @param offset The offset into the buffer to map at.
     * @param size The size of the map.
     */
    void map(vk::DeviceSize offset, vk::DeviceSize size)
    {
        PBL_CORE_ASSERT(size <= m_buffer_size);
        PBL_CORE_ASSERT(!m_buffer_map);

        m_buffer_map = m_device->mapMemory(m_buffer_memory, offset, size);
    }

    /**
     * Unmaps this buffer from host memory.
     */
    void unmap()
    {
        PBL_CORE_ASSERT(m_buffer_map);

        m_device->unmapMemory(m_buffer_memory);
        m_buffer_map = nullptr;
    }

    /**
     * Flush ranges of the buffer.
     */
    void flush(std::vector<vk::MappedMemoryRange>& ranges)
    {
        PBL_CORE_ASSERT(m_buffer_memory);

        m_device->flushMappedMemoryRanges(ranges);
    }

    /**
     * Flush the whole buffer.
     */
    void flush()
    {
        PBL_CORE_ASSERT(m_buffer_memory);

        std::array range = {vk::MappedMemoryRange(m_buffer_memory, 0, m_buffer_size)};
        m_device->flushMappedMemoryRanges(range);
    }

    void copy_from(Buffer& src, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size, vk::CommandBuffer transferCommandBuffer);
    void copy_from(Buffer& src, vk::CommandBuffer transferCommandBuffer);

    void copy_to(Buffer& dst, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size, vk::CommandBuffer transferCommandBuffer);
    void copy_to(Buffer& dst, vk::CommandBuffer transferCommandBuffer);

protected:
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