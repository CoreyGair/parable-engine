#pragma once

#include "Core/Base.h"

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace Parable::Vulkan
{


class CommandPool
{
public:
    CommandPool() = default;
    CommandPool(Device& device, vk::CommandPoolCreateInfo& info)
        : m_device(device)
    {
        m_command_pool = m_device->createCommandPool(info);
    }

    void destroy()
    {
        m_device->destroyCommandPool(m_command_pool);
    }
    
    operator vk::CommandPool&() { return m_command_pool; }
    vk::CommandPool& operator*() { return m_command_pool; }
    vk::CommandPool* operator->() { return &m_command_pool; }

    std::vector<vk::CommandBuffer> create_command_buffers(vk::CommandBufferLevel level, uint32_t count)
    {
        vk::CommandBufferAllocateInfo info(
            m_command_pool,
            level,
            count
        );
        
        return m_device->allocateCommandBuffers(info);
    }
    void free_command_buffers(std::vector<vk::CommandBuffer>& buffers)
    {
        m_device->freeCommandBuffers(m_command_pool, buffers);
    }

    class SingleTimeCommandBuffer
    {
    private:
        SingleTimeCommandBuffer(CommandPool& cmdPool)
            : m_command_pool(cmdPool)
        {
            m_command_buffer = m_command_pool.create_command_buffers(
                vk::CommandBufferLevel::ePrimary,
                1
            )[0];

            m_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        }

    #ifdef PBL_DEBUG
        // in debug mode, add a check to see if submitted before death
    private:
        bool m_has_submitted = false;

    #endif

    public:
        ~SingleTimeCommandBuffer()
        {
            #ifdef PBL_DEBUG
            PBL_CORE_ASSERT(m_has_submitted);
            #endif

            std::vector<vk::CommandBuffer> bufs {m_command_buffer};
            m_command_pool.free_command_buffers(bufs);
        }

        SingleTimeCommandBuffer& operator=(SingleTimeCommandBuffer&& other)
        {
            m_command_pool = other.m_command_pool;
            m_command_buffer = other.m_command_buffer;
            #ifdef PBL_DEBUG
            m_has_submitted = other.m_has_submitted;
            other.m_has_submitted = true;
            #endif
            return *this;
        }
        operator vk::CommandBuffer&() { return m_command_buffer; }

        void end_and_submit(vk::Queue& queue, vk::Fence fence)
        {
            #ifdef PBL_DEBUG
            PBL_CORE_ASSERT(!m_has_submitted);
            #endif

            m_command_buffer.end();

            queue.submit({vk::SubmitInfo(0, nullptr, {}, 1, &m_command_buffer)}, fence);
            //queue.waitIdle();

            m_command_pool.m_device->freeCommandBuffers(m_command_pool, {m_command_buffer});

            #ifdef PBL_DEBUG
            m_has_submitted = true;
            #endif
        }

        void end_and_submit(vk::Queue& queue)
        {
            end_and_submit(queue, vk::Fence{});
        }

    private:
        CommandPool& m_command_pool;

        vk::CommandBuffer m_command_buffer;

        friend CommandPool;
    };
    
    SingleTimeCommandBuffer begin_single_time_command()
    {
        return SingleTimeCommandBuffer(*this);
    }

private:
    Device m_device;

    vk::CommandPool m_command_pool;
};




}