#pragma once

#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{


// TODO: maybe give Device members to construct/destroy the other Vulkan:: wrapper objects,
//      simmilar to how the vk:: wrappers do


/**
 * Wraps vk::Device, the vulkan logical device.
 * 
 * Implicitly casts to vk::Device&, and can be dereferenced to call vk::Device methods.
 */
class Device
{
public:
    Device() = default;
    Device(const Device& other)
    { 
        m_device = other.m_device;
    }
    Device(vk::PhysicalDevice& physicalDevice, vk::DeviceCreateInfo& deviceCreateInfo)
    {
        m_device = physicalDevice.createDevice(deviceCreateInfo);
    }


    operator vk::Device&() { return m_device; }
    vk::Device& operator*() { return m_device; }
    vk::Device* operator->() { return &m_device; }

    /**
     * Destroy the underlying vulkan device.
     */
    void destroy() { m_device.destroy(); }

    vk::DeviceMemory allocate_memory(vk::DeviceSize size, uint32_t type)
    {
        return m_device.allocateMemory(vk::MemoryAllocateInfo(size,type));
    }

    void free_memory(vk::DeviceMemory mem)
    {
        m_device.freeMemory(mem);
    }

private:
    vk::Device m_device;
};


}