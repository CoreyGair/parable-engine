#pragma once

#include "VulkanBase.h"

#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{


class PhysicalDevicePicker;

/**
 * Wraps vk::PhysicalDevice.
 * 
 * Implicitly casts to vk::PhysicalDevice&, and can be dereferenced to call vk::PhysicalDevice methods.
 */
class PhysicalDevice
{
public:
    PhysicalDevice() = default;
    /**
     * Enumerates the avaliable physical devices and picks one.
     * 
     * @param instance The instance to use.
     * @param scorePhysicalDevice The function used to score physical devices.
     */
    PhysicalDevice(vk::Instance& instance, PhysicalDevicePicker& picker);

    operator vk::PhysicalDevice&() { return m_physical_device; }
    vk::PhysicalDevice& operator*() { return m_physical_device; }

    /**
     * Gets the details of the swap chain support for this device and a surface.
     */
    SwapChainSupportDetails get_swapchain_support_details(vk::SurfaceKHR& surface);

    /**
     * Finds the indicies of queue families with certain properties.
     * 
     * @param surface The surface for the queues to work on.
     */
    QueueFamilyIndices get_queue_family_indices(vk::SurfaceKHR& surface);

    /**
     * Finds the indicies of queue families with certain properties.
     * 
     * @param physicalDevice The physical device to examine.
     * @param surface The surface for the queues to work on.
     * @return OptionalQueueFamilyIndices contains optional types for each family index.
     */
    static OptionalQueueFamilyIndices get_optional_queue_family_indices_from_physical_device(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);

    /**
     * Gets the details of the swap chain support for a device and surface pair.
     */
    static SwapChainSupportDetails get_swapchain_support_details_from_physical_device(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);

    uint32_t pick_memory_type(uint32_t suitableTypes, vk::MemoryPropertyFlags requiredProperties);

private:
    vk::PhysicalDevice m_physical_device;
};

/**
 * Allows customiseable physical device picking behaviour based on filters and scorers.
 */
class PhysicalDevicePicker
{
public:
    /**
     * Adds a filter to the picker.
     * 
     * @param filter The filter function, returning false if the physical device cannot be used or true if it can.
     */
    void add_filter(std::function<bool(vk::PhysicalDevice&)>&& filter)
    {
        m_filters.push_back(filter);
    }

    /**
     * Adds a scorer to the picker. Scorer values are summed for each physical device, with the highest score picked.
     * 
     * @param scorer Scorer function; note no score normalisation is applied, so you must ensure the scores are reasonable considering other scorers.
     */
    void add_scorer(std::function<int(vk::PhysicalDevice&)>&& scorer)
    {
        m_scorers.push_back(scorer);
    }
private:
    std::vector<std::function<bool(vk::PhysicalDevice&)>> m_filters;
    std::vector<std::function<int(vk::PhysicalDevice&)>> m_scorers;

    /**
     * Picks the best physical device to use.
    */
    vk::PhysicalDevice& pick(std::vector<vk::PhysicalDevice>& physicalDevices);

    /**
     * PhysicalDevice can use the pick method.
     */
    friend PhysicalDevice;
};


}