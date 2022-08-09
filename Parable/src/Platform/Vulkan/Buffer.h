#pragma once

#include <vulkan/vulkan.h>


namespace Parable::Vulkan
{


class GPU;

class Buffer
{
public:
    Buffer(GPU& gpu, VkBufferCreateInfo& info, VkMemoryPropertyFlags required_memory_properties);
    ~Buffer();

    void write(void* data, VkDeviceSize offset, VkDeviceSize size);

    VkBuffer get_buffer() const { return m_buffer; }
    VkDeviceSize get_size() const { return m_buffer_size; }

    void copy_from(Buffer& src, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size, VkCommandBuffer transfer_command_buffer);
    void copy_from(Buffer& src, VkCommandBuffer transfer_command_buffer);

    void copy_to(Buffer& dst, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size, VkCommandBuffer transfer_command_buffer);
    void copy_to(Buffer& dst, VkCommandBuffer transfer_command_buffer);

private:
    GPU& m_gpu;

    VkBuffer m_buffer;
    VkDeviceSize m_buffer_size;
    VkDeviceMemory m_buffer_memory;
};

class BufferBuilder
{
public:
    BufferBuilder()
    {
        buffer_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        };
    }

    std::unique_ptr<Buffer> create(GPU& gpu);

    VkBufferCreateInfo buffer_info;
    VkMemoryPropertyFlags required_memory_properties = 0;
};


}