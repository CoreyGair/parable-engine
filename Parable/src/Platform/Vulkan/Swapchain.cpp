#include "Swapchain.h"

#include "Core/Base.h"

#include "GLFW/glfw3.h"

#include "VulkanExceptions.h"

#include "GPU.h"


namespace Parable::Vulkan
{


Swapchain::Swapchain(GPU& gpu, GLFWwindow* window)
    : m_gpu(gpu)
{
    create_swapchain(window);
}

Swapchain::~Swapchain()
{
    destroy_swapchain();
}

void Swapchain::recreate_swapchain(GLFWwindow* window)
{
    // ensure we arent processing anything before recreating
    m_gpu.wait_for_idle();

    destroy_swapchain();

    create_swapchain(window);
}

void Swapchain::destroy_swapchain()
{
    for (size_t i = 0; i < m_swapchain_image_views.size(); i++) {
        vkDestroyImageView(m_gpu.device, m_swapchain_image_views[i], nullptr);
    }

    vkDestroySwapchainKHR(m_gpu.device, m_swapchain, nullptr);
}

void Swapchain::create_swapchain(GLFWwindow* window)
{
    const SwapChainSupportDetails& swap_chain_support = m_gpu.get_swapchain_support_details();
    const QueueFamilyIndices& queue_indices = m_gpu.get_queue_family_indices();

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
    VkExtent2D extent = choose_swap_extent(window, swap_chain_support.capabilities);

    PBL_CORE_TRACE("Vulkan: Swap extent ({},{}).", extent.width, extent.height);

    // the number of images in the swap chain queue
    // request more than the min to reduce waits on driver
    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    // actually create the swap chain
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_gpu.render_surface;
    createInfo.minImageCount = image_count;
    createInfo.imageFormat = surface_format.format;
    createInfo.imageColorSpace = surface_format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] = {queue_indices.graphics_family.value(), queue_indices.presentation_family.value()};
    if (queue_indices.graphics_family != queue_indices.presentation_family) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queue_family_indices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present_mode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult res = vkCreateSwapchainKHR(m_gpu.device, &createInfo, nullptr, &m_swapchain);
    if (res != VK_SUCCESS) {
        throw VulkanFailedCreateException("swapchain", res);
    }
    
    vkGetSwapchainImagesKHR(m_gpu.device, m_swapchain, &image_count, nullptr);
    m_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_gpu.device, m_swapchain, &image_count, m_images.data());

    m_image_format = surface_format.format;
    m_extent = extent;

    create_views();
}

VkSurfaceFormatKHR Swapchain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
    for (const auto& available_format : available_formats) {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }

    return available_formats[0];
}

VkPresentModeKHR Swapchain::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) {
    for (const auto& available_present_mode : available_present_modes) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::choose_swap_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        // if no special extents, return the surface extents
        return capabilities.currentExtent;
    } else {
        // calculate the best extents to use (closest to window size within limits)
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actual_extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actual_extent;
    }
}

void Swapchain::create_views()
{
    m_swapchain_image_views.resize(m_images.size());

    // create an image view for each image in the swap chain
    for (size_t i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_image_format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult res = vkCreateImageView(m_gpu.device, &createInfo, nullptr, &m_swapchain_image_views[i]);
        if (res != VK_SUCCESS)
        {
            throw VulkanFailedCreateException("image view", res);
        }
    }
}


}