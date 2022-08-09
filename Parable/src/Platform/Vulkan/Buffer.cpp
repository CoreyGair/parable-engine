#include "Buffer.h"

#include "Core/Base.h"

#include "Platform/Vulkan/GPU.h"
#include "VulkanExceptions.h"


namespace Parable::Vulkan
{


Buffer::Buffer(GPU& gpu, VkBufferCreateInfo& info, VkMemoryPropertyFlags required_memory_properties) : m_gpu(gpu)
{
    m_buffer_size = info.size;

    VkResult result = vkCreateBuffer(m_gpu.device, &info, nullptr, &m_buffer);

    if (result != VK_SUCCESS) {
        throw VulkanFailedCreateException("vertex buffer", result);
    }

    // alloc mem from gpu
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(m_gpu.device, m_buffer, &mem_requirements);

    uint32_t mem_type = m_gpu.pick_memory_type(mem_requirements.memoryTypeBits, required_memory_properties);

    m_buffer_memory = m_gpu.allocate_device_memory(mem_requirements.size, mem_type);

    // bind mem to buffer
    vkBindBufferMemory(m_gpu.device, m_buffer, m_buffer_memory, 0);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(m_gpu.device, m_buffer, nullptr);
    m_gpu.free_device_memory(m_buffer_memory);
}

/**
 * Write some data into this buffer.
 * 
 * Note this assumes the buffer is host coherent; it does not explicitly flush.
 * 
 * @param data The data to write.
 * @param offset The offset into the buffer to write at.
 * @param size The size of data to be written.
 */
void Buffer::write(void* data, VkDeviceSize offset, VkDeviceSize size)
{
    PBL_CORE_ASSERT(size <= m_buffer_size);

    void* buffer_map;
    vkMapMemory(m_gpu.device, m_buffer_memory, offset, size, 0, &buffer_map);
    memcpy(buffer_map, data, (size_t)size);
    vkUnmapMemory(m_gpu.device, m_buffer_memory);
}

/**
 * Creates a vulkan transfer command to copy data from another buffer to this one.
 * 
 * @param src The source buffer to copy from.
 * @param src_offset The offset into src to copy from
 * @param dst_offset The offset into this buffer to copy to.
 * @param size The size of data to copy.
 * @param transfer_command_buffer The command buffer to place the transfer command into.
 */
void Buffer::copy_from(Buffer& src, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size, VkCommandBuffer transfer_command_buffer)
{
    PBL_CORE_ASSERT(m_buffer_size >= size);
    PBL_CORE_ASSERT(src.m_buffer_size >= src_offset + size);

    VkBufferCopy copy_region = {
        .srcOffset = src_offset,
        .dstOffset = dst_offset,
        .size = size,
    };
    
    vkCmdCopyBuffer(transfer_command_buffer, src.m_buffer, m_buffer, 1, &copy_region);
}

/**
 * Creates a vulkan transfer command to copy data from another buffer into this one.
 * 
 * Copies all the data from src to this buffer.
 * 
 * @param dst The source buffer to copy from.
 * @param transfer_command_buffer The command buffer to place the transfer command into.
 */
void Buffer::copy_from(Buffer& src, VkCommandBuffer transfer_command_buffer)
{
    copy_from(src, 0, 0, src.m_buffer_size, transfer_command_buffer);
}

/**
 * Creates a vulkan transfer command to copy data from this buffer to another one.
 * 
 * @param dst The destination buffer to copy into.
 * @param src_offset The offset into this buffer to copy from
 * @param dst_offset The offset into dst to copy to.
 * @param size The size of data to copy.
 * @param transfer_command_buffer The command buffer to place the transfer command into.
 */
void Buffer::copy_to(Buffer& dst, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size, VkCommandBuffer transfer_command_buffer)
{
    dst.copy_from(*this, dst_offset, src_offset, size, transfer_command_buffer);
}

/**
 * Creates a vulkan transfer command to copy data from this buffer to another one.
 * 
 * Copies all the data from this buffer into the dst.
 * 
 * @param dst The destination buffer to copy into.
 * @param transfer_command_buffer The command buffer to place the transfer command into.
 */
void Buffer::copy_to(Buffer& dst, VkCommandBuffer transfer_command_buffer)
{
    dst.copy_from(*this, transfer_command_buffer);
}

std::unique_ptr<Buffer> BufferBuilder::create(GPU& gpu)
{
    return std::make_unique<Buffer>(gpu, buffer_info, required_memory_properties);
}


}