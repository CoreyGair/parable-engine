#pragma once

#include <vulkan/vulkan.h>
#include <pblpch.h>


class GLFWwindow;

namespace Parable::Vulkan
{


/**
 * Stores the family indices of queue families we are interested in.
 */
struct QueueFamilyIndices
{
    // if a family is added, ensure get_queue_family_indices is updated so we actually find it
    // also update is_complete and unique_values
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> presentation_family;
    std::optional<uint32_t> transfer_family;

    bool is_complete() const { return graphics_family.has_value() && presentation_family.has_value() && transfer_family.has_value(); }

    std::set<uint32_t> unique_values() const { return { graphics_family.value(), presentation_family.value(), transfer_family.value() }; }
};

/**
 * Stores details of the swap chain support for a (device, surface) pair.
 */
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

/**
 * Groups together vulkan handles associated with a GPU.
 */
class GPU
{
public:
    GPU(VkInstance _instance, VkSurfaceKHR _render_surface, VkPhysicalDevice _physical_device, VkDevice _device, QueueFamilyIndices queue_family_indices)
        : instance(_instance),
        render_surface(_render_surface),
        physical_device(_physical_device),
        device(_device),
        m_queue_family_indices(queue_family_indices)
    {}

    ~GPU();

    void wait_for_idle();

    const QueueFamilyIndices& get_queue_family_indices() const { return m_queue_family_indices; }

    SwapChainSupportDetails get_swapchain_support_details() const;

    uint32_t pick_memory_type(uint32_t suitable_types, VkMemoryPropertyFlags required_properties);

    VkDeviceMemory allocate_device_memory(VkDeviceSize size, uint32_t type);
    void free_device_memory(VkDeviceMemory mem);

    const VkInstance instance;
    const VkSurfaceKHR render_surface;
    const VkPhysicalDevice physical_device;
    const VkDevice device;

private:
    QueueFamilyIndices m_queue_family_indices;
};

class GPUBuilder
{
public:

    std::unique_ptr<GPU> create(GLFWwindow* render_window);

    std::vector<const char*> validation_layers;

    std::vector<const char*> required_extensions;

    std::vector<const char*> device_extensions;

    const char* app_name = "";

private:
    bool check_validation_layer_support();
    VkInstance create_instance();
    VkPhysicalDevice pick_physical_device(VkInstance instance, VkSurfaceKHR surface);
    bool check_device_extension_support(VkPhysicalDevice device);
    SwapChainSupportDetails get_swapchain_support(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    QueueFamilyIndices get_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
    VkDevice create_device(VkPhysicalDevice physical_device, const QueueFamilyIndices& queue_indices);
};


}