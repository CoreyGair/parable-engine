#include "Buffer.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"



namespace Parable::Vulkan
{


Buffer::Buffer(Device& device, PhysicalDevice& physicalDevice, vk::BufferCreateInfo& info, vk::MemoryPropertyFlags requiredMemoryProperties) : m_device(device)
{
    m_buffer_size = info.size;

    m_buffer = (*m_device).createBuffer(info);

    // alloc mem from gpu
    vk::MemoryRequirements memRequirements = (*m_device).getBufferMemoryRequirements(m_buffer);

    uint32_t memType = physicalDevice.pick_memory_type(memRequirements.memoryTypeBits, requiredMemoryProperties);

    m_buffer_memory = m_device.allocate_memory(memRequirements.size, memType);

    // bind mem to buffer
    (*m_device).bindBufferMemory(m_buffer, m_buffer_memory, 0);
}

/**
 * Creates a vulkan transfer command to copy data from another buffer to this one.
 * 
 * @param src The source buffer to copy from.
 * @param srcOffset The offset into src to copy from
 * @param dstOffset The offset into this buffer to copy to.
 * @param size The size of data to copy.
 * @param transferCommandBuffer The command buffer to place the transfer command into.
 */
void Buffer::copy_from(Buffer& src, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size, vk::CommandBuffer transferCommandBuffer)
{
    PBL_CORE_ASSERT(m_buffer_size >= size);
    PBL_CORE_ASSERT(src.m_buffer_size >= srcOffset + size);

    vk::BufferCopy bufCopy(srcOffset, dstOffset, size);

    transferCommandBuffer.copyBuffer(src, m_buffer, 1, &bufCopy);
}

/**
 * Creates a vulkan transfer command to copy data from another buffer into this one.
 * 
 * Copies all the data from src to this buffer.
 * 
 * @param src The source buffer to copy from.
 * @param transferCommandBuffer The command buffer to place the transfer command into.
 */
void Buffer::copy_from(Buffer& src, vk::CommandBuffer transferCommandBuffer)
{
    copy_from(src, 0, 0, src.m_buffer_size, transferCommandBuffer);
}

/**
 * Creates a vulkan transfer command to copy data from this buffer to another one.
 * 
 * @param dst The destination buffer to copy into.
 * @param srcOffset The offset into this buffer to copy from
 * @param dstOffset The offset into dst to copy to.
 * @param size The size of data to copy.
 * @param transferCommandBuffer The command buffer to place the transfer command into.
 */
void Buffer::copy_to(Buffer& dst, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize size, vk::CommandBuffer transferCommandBuffer)
{
    dst.copy_from(*this, dstOffset, srcOffset, size, transferCommandBuffer);
}

/**
 * Creates a vulkan transfer command to copy data from this buffer to another one.
 * 
 * Copies all the data from this buffer into the dst.
 * 
 * @param dst The destination buffer to copy into.
 * @param transferCommandBuffer The command buffer to place the transfer command into.
 */
void Buffer::copy_to(Buffer& dst, vk::CommandBuffer transferCommandBuffer)
{
    dst.copy_from(*this, transferCommandBuffer);
}

Buffer BufferBuilder::create(Device& device, PhysicalDevice& physicalDevice)
{
    return Buffer(device, physicalDevice, buffer_info, required_memory_properties);
}

/**
 * Emplace the new buffer at the back of a vector.
 * 
 * @param vec The vector to emplace into.
 * @param gpu The device used to create the vk buffer.
 */
void BufferBuilder::emplace_back_vector(std::vector<Buffer>& vec, Device& device, PhysicalDevice& physicalDevice)
{
    vec.emplace_back(device, physicalDevice, buffer_info, required_memory_properties);
}


}