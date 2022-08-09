#include "CommandPool.h"

#include "Platform/Vulkan/GPU.h"

#include "Platform/Vulkan/VulkanExceptions.h"

namespace Parable::Vulkan
{


CommandPool::CommandPool(GPU& gpu, VkCommandPoolCreateInfo& info) : m_gpu(gpu)
{
    VkResult result = vkCreateCommandPool(m_gpu.device, &info, nullptr, &m_command_pool);
    if (result != VK_SUCCESS) {
        throw VulkanFailedCreateException("command pool", result);
    }
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(m_gpu.device, m_command_pool, nullptr);
}

std::vector<VkCommandBuffer> CommandPool::create_command_buffers(VkCommandBufferLevel level, uint32_t count)
{
    std::vector<VkCommandBuffer> buffers;
    buffers.resize(count);

    VkCommandBufferAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_command_pool,
        .level = level,
        .commandBufferCount = count,
    };
    
    VkResult result = vkAllocateCommandBuffers(m_gpu.device, &info, buffers.data());
    if (result != VK_SUCCESS) {
        throw VulkanFailedCreateException("command buffer", result);
    }

    return std::move(buffers);
}

void CommandPool::free_command_buffers(std::vector<VkCommandBuffer>& buffers)
{
    vkFreeCommandBuffers(m_gpu.device, m_command_pool, buffers.size(), buffers.data());
}


}

