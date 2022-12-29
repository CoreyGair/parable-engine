#pragma once

#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{


/**
 * Wraps vk::Device, the vulkan logical device.
 * 
 * Implicitly casts to vk::Device&, and can be dereferenced to call vk::Device methods.
 */
class Device
{
public:
    Device() = default;
    Device(vk::PhysicalDevice& physicalDevice, vk::DeviceCreateInfo& deviceCreateInfo);

    operator vk::Device&() { return m_device; }
    vk::Device& operator*() { return m_device; }

    /**
     * Destroy the underlying vulkan device.
     */
    void destroy() { m_device.destroy(); }

private:
    vk::Device m_device;
};


}