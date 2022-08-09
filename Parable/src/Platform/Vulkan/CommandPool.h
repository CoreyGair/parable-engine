#pragma once

#include "Core/Base.h"

#include <vulkan/vulkan.h>

namespace Parable::Vulkan
{


class GPU;

class CommandPool
{
public:
    CommandPool(GPU& gpu, VkCommandPoolCreateInfo& info);
    ~CommandPool();
    
    VkCommandPool get_command_pool() const { return m_command_pool; }

    std::vector<VkCommandBuffer> create_command_buffers(VkCommandBufferLevel level, uint32_t count);
    void free_command_buffers(std::vector<VkCommandBuffer>& buffers);

private:
    GPU& m_gpu;

    VkCommandPool m_command_pool;
};


}