#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "../Wrapper/PhysicalDevice.h"
#include "../Wrapper/Device.h"
#include "../Wrapper/CommandPool.h"
#include "../Wrapper/CommandPool.h"

#include "LoadTask.h"

namespace Parable::Vulkan
{


/**
 * Queues loading tasks for vulkan resources and dispatches tasks in bulk.
 */
class Loader
{
private:
    std::vector<std::unique_ptr<LoadTask>> m_tasks;

    PhysicalDevice m_physical_device;
    Device m_device;
    
    vk::Queue m_transfer_queue;

    CommandPool m_command_pool;
    vk::CommandBuffer m_command_buffer;

    vk::Fence m_transfer_finished_fence;

public:
    Loader(Device device, PhysicalDevice physical_device, uint32_t transfer_family);
    ~Loader();

    /**
     * Submit a load task to be dispatched at some point in the future.
     * 
     * @param task A unique pointer to the task object.
     */
    void submit_task(std::unique_ptr<LoadTask> task);

    void run_tasks();
};


}
