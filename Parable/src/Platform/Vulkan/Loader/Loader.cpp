#include "Loader.h"

#include <vulkan/vulkan.hpp>

#include "LoadTask.h"

namespace Parable::Vulkan
{


Loader::Loader(Device device, PhysicalDevice physical_device, uint32_t transfer_family)
    : m_device(device),
    m_physical_device(physical_device),
    m_transfer_queue(device->getQueue(transfer_family, 0))
{
    vk::CommandPoolCreateInfo cmd_pool_info(   
        vk::CommandPoolCreateFlagBits::eTransient,
        transfer_family
    );
    m_command_pool = CommandPool(m_device, cmd_pool_info);

    m_command_buffer = m_command_pool.create_command_buffers(vk::CommandBufferLevel::ePrimary, 1)[0];

    vk::FenceCreateInfo fenceInfo {};
    m_transfer_finished_fence = m_device->createFence(fenceInfo);
}

Loader::~Loader()
{
    m_device->destroyFence(m_transfer_finished_fence);

    std::vector<vk::CommandBuffer> buffs{m_command_buffer};
    m_command_pool.free_command_buffers(buffs);
    m_command_pool.destroy();
}

void Loader::submit_task(std::unique_ptr<LoadTask> task)
{
    m_tasks.push_back(std::move(task));
}

void Loader::run_tasks()
{
    m_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

    for (auto& task : m_tasks)
    {
        task->record_commands(m_device, m_physical_device, m_command_buffer);
    }

    m_command_buffer.end();

    m_transfer_queue.submit({vk::SubmitInfo(0, nullptr, nullptr, 1, &m_command_buffer)}, m_transfer_finished_fence);

    // call all the host/driver side creation funcs while the GPU works
    for (auto& task : m_tasks)
    {
        task->create_device_resources();
    }

    // now wait until the GPU is actually done
    vk::Result res = m_device->waitForFences(1, &m_transfer_finished_fence, VK_TRUE, UINT64_MAX);
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for transfer fence.");
    }

    // cleanup
    for (auto& task : m_tasks)
    {
        task->on_load_complete();
    }

    res = m_device->resetFences(1, &m_transfer_finished_fence);
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to reset transfer fence.");
    }

    m_device->resetCommandPool(m_command_pool);

    m_tasks.clear();
}


}
