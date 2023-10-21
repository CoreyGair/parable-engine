#pragma once

namespace vk
{
    class CommandBuffer;
}

namespace Parable::Vulkan
{


class Device;
class PhysicalDevice;

/**
 * Interface for a single vulkan resource loading job.
 */
class LoadTask
{
public:
    virtual ~LoadTask() = 0;

    // All the interface methods are pure virtual to force implementations
    // to explicitly acknowledge they do not use a specific function by defining it empty.

    /**
     * Record the necessary vulkan commands to do this load.
     * 
     * This is used by all resource loading tasks which must execute some GPU commands to load.
     * 
     * Note that currently, the command buffer queue is only guaranteed to have transfer capabilities
     * (e.g. implementations should not record any graphics, compute or non-transfer queue commands).
     * 
     * @param command_buffer The command buffer to record into.
     */
    virtual void record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer) = 0;

    /**
     * Create resources synchronously using the vulkan device.
     */
    virtual void create_device_resources(Device& device) = 0;

    /**
     * Called when all async loading task is complete, signalling that any needed cleanup can occur.
     */
    virtual void on_load_complete() = 0;
};


}
