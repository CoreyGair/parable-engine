#pragma once

#include <pblpch.h>

#include <vulkan/vulkan.hpp>

#include "VulkanExceptions.h"


/**
 * @file Defines some common structs for the Vulkan wrappers.
 */

namespace Parable::Vulkan
{

/**
 * Stores the family indices of queues we are interested in.
 */
struct QueueFamilyIndices
{
    uint32_t graphics_family;
    uint32_t presentation_family;
    uint32_t transfer_family;

    std::set<uint32_t> unique_values() const { return { graphics_family, presentation_family, transfer_family }; }
};

/**
 * Stores family indicies in optionals.
 */
struct OptionalQueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> presentation_family;
    std::optional<uint32_t> transfer_family;

    explicit operator QueueFamilyIndices() { return QueueFamilyIndices{graphics_family.value(), presentation_family.value(), transfer_family.value()}; }

    bool is_complete() const { return graphics_family.has_value() && presentation_family.has_value() && transfer_family.has_value(); }

    std::set<uint32_t> unique_values() const { return { graphics_family.value(), presentation_family.value(), transfer_family.value() }; }
};

/**
 * Stores details of the swap chain support for a (device, surface) pair.
 */
struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> present_modes;
};


}