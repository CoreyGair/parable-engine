#pragma once

#include "Core/Base.h"

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace Parable::Vulkan
{


class CommandPool
{
public:
    CommandPool(Device& device, vk::CommandPoolCreateInfo& info);

    void destroy()
    {
        m_device.destroyCommandPool(m_command_pool);
    }
    
    operator vk::CommandPool&() { return m_command_pool; }
    vk::CommandPool& operator*() { return m_command_pool; }

    std::vector<vk::CommandBuffer> create_command_buffers(vk::CommandBufferLevel level, uint32_t count);
    void free_command_buffers(std::vector<vk::CommandBuffer>& buffers);

private:
    Device m_device;

    vk::CommandPool m_command_pool;
};


}