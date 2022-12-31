#include "CommandPool.h"

#include "Platform/Vulkan/GPU.h"

#include "Platform/Vulkan/VulkanExceptions.h"

namespace Parable::Vulkan
{


CommandPool::CommandPool(Device& device, vk::CommandPoolCreateInfo& info)
    : m_device(device)
{
    m_command_pool = m_device.createCommandPool(info);
}

std::vector<vk::CommandBuffer> CommandPool::create_command_buffers(vk::CommandBufferLevel level, uint32_t count)
{
    std::vector<vk::CommandBuffer> buffers;
    buffers.resize(count);

    vk::CommandBufferAllocateInfo info(
        m_command_pool,
        level,
        count
    );
    
    m_device.allocateCommandBuffers(info, buffers.data());

    return std::move(buffers);
}

void CommandPool::free_command_buffers(std::vector<VkCommandBuffer>& buffers)
{
    m_device.freeCommandBuffers(m_command_pool, buffers);
}


}

