#include "GPU.h"

#include <pblpch.h>
#include "Core/Base.h"

#include <vulkan/vulkan.h>
#include "VulkanExceptions.h"

#include "GLFW/glfw3.h"


namespace Parable::Vulkan
{


GPU::~GPU()
{
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, render_surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void GPU::wait_for_idle()
{
    vkDeviceWaitIdle(device);
}

/**
 * Gets the details of the swap chain support for this GPUs (physical device, surface) pair.
 */
SwapChainSupportDetails GPU::get_swapchain_support_details() const
{
    SwapChainSupportDetails details;

    // surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, render_surface, &details.capabilities);

    // surface formats
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, render_surface, &format_count, nullptr);

    details.formats.resize(format_count);
    if (format_count != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, render_surface, &format_count, details.formats.data());
    }

    // presentation modes
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, render_surface, &present_mode_count, nullptr);

    details.present_modes.resize(present_mode_count);
    if (present_mode_count != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, render_surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

uint32_t GPU::pick_memory_type(uint32_t suitable_types, VkMemoryPropertyFlags required_properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) {
        // check if the type is in the suitable types
        // and if all the required properties are available
        if (
            (suitable_types & (1 << i)) &&
            ((mem_properties.memoryTypes[i].propertyFlags & required_properties) == required_properties)
        ) {
            return i;
        }
    }

    throw VulkanResourceException("No suitable memory type.");
}

VkDeviceMemory GPU::allocate_device_memory(VkDeviceSize size, uint32_t type)
{
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = size;
    alloc_info.memoryTypeIndex = type;

    VkDeviceMemory mem;
    VkResult result = vkAllocateMemory(device, &alloc_info, nullptr, &mem);

    if (result != VK_SUCCESS)
    {
        throw VulkanDeviceAllocationFailed(result);
    }

    return mem;
}

void GPU::free_device_memory(VkDeviceMemory mem)
{
    vkFreeMemory(device, mem, nullptr);
}


/// GPUBuilder

std::unique_ptr<GPU> GPUBuilder::create(GLFWwindow* render_window)
{
    VkInstance instance = create_instance();

    VkSurfaceKHR render_surface;
    glfwCreateWindowSurface(instance, render_window, nullptr, &render_surface);

    VkPhysicalDevice physical_device = pick_physical_device(instance, render_surface);

    QueueFamilyIndices queue_indices = get_queue_family_indices(physical_device, render_surface);

    VkDevice device = create_device(physical_device, queue_indices);

    return std::make_unique<GPU>(instance, render_surface, physical_device, device, queue_indices);
}

/**
 * Checks that vulkan supports the validation layers we want.
 */
bool GPUBuilder::check_validation_layer_support()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : validation_layers) {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

/**
 * Creates the vulkan instance.
 */
VkInstance GPUBuilder::create_instance()
{
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // TODO: set this to the game name somehow
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Parable Engine";
    // TODO: set to parable version
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info
    };

    // find required glfw extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    // add the glfw extensions to the other required extensions
    required_extensions.reserve(required_extensions.size() + glfw_extension_count);
    for (size_t i = 0; i < glfw_extension_count; ++i)
    {
        const char* glfw_ext = glfw_extensions[i];
        if (find_if(
                required_extensions.cbegin(),
                required_extensions.cend(),
                [&glfw_ext](const char* ext) { return strcmp(ext,glfw_ext) == 0; }
            ) 
            == required_extensions.cend())
        {
            required_extensions.push_back(glfw_ext);
        }
    }

    // attach extensions
    instance_create_info.enabledExtensionCount = required_extensions.size();
    instance_create_info.ppEnabledExtensionNames = required_extensions.data();

    // attach validation layers
    if (validation_layers.size() != 0) {
        PBL_CORE_ASSERT_MSG(check_validation_layer_support(), "Vulkan validation layers requested but not supported!");
        instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        instance_create_info.enabledLayerCount = 0;
    }

    VkInstance instance;
    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw VulkanFailedCreateException("instance", result);
    }

    return instance;
}

VkPhysicalDevice GPUBuilder::pick_physical_device(VkInstance instance, VkSurfaceKHR surface)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0)
    {
        throw VulkanResourceException("No physical devices avaliable!");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    // pick a physical device
    uint64_t bestScore = 0;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);

        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(device, &device_features);

        VkPhysicalDeviceMemoryProperties device_memory;
        vkGetPhysicalDeviceMemoryProperties(device, &device_memory);

        // device MUST have these
        // geom shaders
        if (!device_features.geometryShader) {
            PBL_CORE_TRACE("Device {} does not have geometry shaders.", device_properties.deviceName);
            continue;
        }

        // queue families
        QueueFamilyIndices queue_indices = get_queue_family_indices(device, surface);
        if (!queue_indices.is_complete()) {
            PBL_CORE_TRACE("Device {}: issue with queues. Graphics queue {}. Presentation queue {}.",
                device_properties.deviceName,
                queue_indices.graphics_family.has_value() ? "found" : "not found",
                queue_indices.presentation_family.has_value() ? "found" : "not found");
            continue;
        }

        // device extensions
        bool extensions_supported = check_device_extension_support(device);
        if (!extensions_supported) {
            PBL_CORE_TRACE("Device {} does not support required extensions.", device_properties.deviceName);
            continue;
        }

        // swap chain features
        bool swap_chain_adequate = false;
        SwapChainSupportDetails swap_chain_support = get_swapchain_support(device, surface);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
        if (!swap_chain_adequate) {
            PBL_CORE_TRACE("Device {} does not have required swap chain features. Has {} formats and {} presentation modes.",
                device_properties.deviceName,
                swap_chain_support.formats.size(),
                swap_chain_support.present_modes.size()
            );
            continue;
        }

        // devices CAN have these, used to make a score to rank choices
        // value discrete gpus highly
        uint64_t score = 0;
        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 100000;

        // try to pick the device with the most mem (good metric for performance)
        auto heaps_pointer = device_memory.memoryHeaps;
        auto heaps = std::vector<VkMemoryHeap>(heaps_pointer, heaps_pointer + device_memory.memoryHeapCount);
        VkDeviceSize max_mem_size = 0;
        for (const auto& heap : heaps)
        {
            if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                // Device local heap, should be size of total GPU VRAM.
                // heap.size will be the size of VRAM in bytes. (bigger is better)
                if (heap.size > max_mem_size) max_mem_size = heap.size;
            }
        }
        score += max_mem_size;
        PBL_CORE_TRACE("Physical device {}, with memory {}.", device_properties.deviceName, max_mem_size);

        if (score > bestScore)
        {
            bestScore = score;
            physical_device = device;
        }
    }

    if (physical_device == VK_NULL_HANDLE)
    {
        throw NoGraphicsDeviceException();
    }

    return physical_device;
}

/**
 * Checks the physcal device supports the extensions we want.
 */
bool GPUBuilder::check_device_extension_support(VkPhysicalDevice device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

/**
 * Gets the details of the swap chain support for a (physical device, surface) pair.
 * 
 * @param physical_device The physical device to query.
 * @param surface The surface the swap chain needs to present to.
 */
SwapChainSupportDetails GPUBuilder::get_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    // surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

    // surface formats
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);

    details.formats.resize(format_count);
    if (format_count != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats.data());
    }

    // presentation modes
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

    details.present_modes.resize(present_mode_count);
    if (present_mode_count != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

/**
 * Finds the indicies of queue families with certain properties.
 * 
 * @param physical_device The physical device to check the queue families of.
 * @param surface The surface for the presentation queue to work on.
 */
QueueFamilyIndices GPUBuilder::get_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    // query the device queue families
    uint32_t device_queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> device_queue_families(device_queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_count, device_queue_families.data());

    uint32_t i = 0;
    for (const auto& queue_family : device_queue_families) {
        // find a graphics queue
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        // find a queue able to present to the surface
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        if (present_support) {
            indices.presentation_family = i;
        }

        // find a dedicated transfer queue
        if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT && !(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            indices.transfer_family = i;
        }

        if (indices.is_complete()) {
            break;
        }

        i++;
    }

    // graphics and compute queues implicitly support transfer ops
    // so, if we dont have a dedicated transfer queue, use the graphics one
    if (!indices.transfer_family.has_value())
    {
        indices.transfer_family = indices.graphics_family;
    }

    return indices;
}

/**
 * Creates the vulkan logical device.
 */
VkDevice GPUBuilder::create_device(VkPhysicalDevice physical_device, const QueueFamilyIndices& queue_indices)
{
    if (!queue_indices.is_complete())
    {
        throw VulkanResourceException("Incomplete queue family indices object.");
    }

    std::set<uint32_t> unique_queue_families = queue_indices.unique_values();
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    // setup the queue creation data
    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    // specify the device features we need
    VkPhysicalDeviceFeatures device_features{};

    // creating the actual logical device
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();
    if (validation_layers.size() != 0) {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    VkDevice device;
    VkResult res = vkCreateDevice(physical_device, &create_info, nullptr, &device);
    if (res != VK_SUCCESS)
    {
        throw VulkanFailedCreateException("device", res);
    }

    return device;
}


}