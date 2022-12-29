#include "Device.h"

#include "VulkanBase.h"

#include <pblpch.h>

#include "Core/Base.h"


namespace Parable::Vulkan
{


Device::Device(vk::PhysicalDevice& physicalDevice, vk::DeviceCreateInfo& deviceCreateInfo)
{
    m_device = physicalDevice.createDevice(deviceCreateInfo);
}


}