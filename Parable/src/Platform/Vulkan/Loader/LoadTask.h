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
    /**
     * Record the necessary vulkan commands to do this load.
     * 
     * Note that currently, the command buffer queue is only guaranteed to have transfer capabilities
     * (e.g. implementations should not record any graphics, compute or non-transfer queue commands).
     * 
     * @param command_buffer The command buffer to record into.
     */
    virtual void record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer) = 0;

    /**
     * Called when the loading task is complete, signalling that any needed cleanup can occur.
     */
    virtual void on_load_complete() = 0;
};


}
