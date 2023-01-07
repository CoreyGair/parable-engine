#include "PhysicalDevice.h"

#include "VulkanBase.h"

#include <vulkan/vulkan.hpp>

#include <pblpch.h>

#include "Core/Base.h"

#include <limits>

namespace Parable::Vulkan
{


PhysicalDevice::PhysicalDevice(vk::Instance& instance, PhysicalDevicePicker& picker)
{
    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

    if (physicalDevices.size() == 0)
    {
        throw VulkanResourceException("No physical devices avaliable!");
    }

    m_physical_device = picker.pick(physicalDevices);
}

SwapChainSupportDetails PhysicalDevice::get_swapchain_support_details(vk::SurfaceKHR& surface)
{
    return get_swapchain_support_details_from_physical_device(m_physical_device, surface);
}

class InvalidQueueFamilies : public Exception
{
public:
    InvalidQueueFamilies(vk::PhysicalDevice& pd)
        : Exception(std::format("Physical device {} queue family indices invalid.", pd.getProperties().deviceName)) {}
};

QueueFamilyIndices PhysicalDevice::get_queue_family_indices(vk::SurfaceKHR& surface)
{
    OptionalQueueFamilyIndices indices = get_optional_queue_family_indices_from_physical_device(m_physical_device, surface);

    // graphics and compute queues implicitly support transfer ops
    // so, if we dont have a dedicated transfer queue, use the graphics one
    if (!indices.transfer_family.has_value()) {
        indices.transfer_family = indices.graphics_family;
    }

    if (!indices.is_complete()) {
        throw InvalidQueueFamilies(m_physical_device);
    }

    return (QueueFamilyIndices)indices;
}

OptionalQueueFamilyIndices PhysicalDevice::get_optional_queue_family_indices_from_physical_device(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface)
{
    OptionalQueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

    uint32_t i = 0;
    for (const auto& qf : queueFamilies) {
        // find a graphics queue
        if (qf.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphics_family = i;
        }

        // find a queue able to present to the surface
        VkBool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, surface);
        if (presentSupport) {
            indices.presentation_family = i;
        }

        // find a dedicated transfer queue
        if (qf.queueFlags & vk::QueueFlagBits::eTransfer && !(qf.queueFlags & vk::QueueFlagBits::eGraphics)) {
            indices.transfer_family = i;
        }

        if (indices.is_complete()) {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainSupportDetails PhysicalDevice::get_swapchain_support_details_from_physical_device(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface)
{
    SwapChainSupportDetails details{
        .capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface),
        .formats = physicalDevice.getSurfaceFormatsKHR(surface),
        .present_modes = physicalDevice.getSurfacePresentModesKHR(surface),
    };

    return details;
}

uint32_t PhysicalDevice::pick_memory_type(uint32_t suitableTypes, vk::MemoryPropertyFlags requiredProperties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = m_physical_device.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        // check if the type is in the suitable types
        // and if all the required properties are available
        if (
            (suitableTypes & (1 << i)) &&
            ((memProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)
        ) {
            return i;
        }
    }

    throw VulkanResourceException("No suitable memory type.");
}

vk::PhysicalDevice& PhysicalDevicePicker::pick(std::vector<vk::PhysicalDevice>& physicalDevices)
{
    int maxScore = std::numeric_limits<int>::min();
    int best = 0;

    for (int i = 0; i < physicalDevices.size(); ++i) {
        for (auto& filter : m_filters) {
            if (!filter(physicalDevices[i])) {
                continue;
            }
        }

        int score = 0;
        for (auto& scorer : m_scorers) {
            score += scorer(physicalDevices[i]);
        }

        if (score > maxScore) {
            maxScore = score;
            best = i;
        }
    }

    return physicalDevices[best];
}


}