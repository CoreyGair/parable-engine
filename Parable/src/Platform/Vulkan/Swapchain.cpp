#include "Swapchain.h"

#include "VulkanBase.h"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "Core/Base.h"

#include "GLFW/glfw3.h"

#include "VulkanExceptions.h"

#include "Device.h"
#include "PhysicalDevice.h"


namespace Parable::Vulkan
{


Swapchain::Swapchain(Device& device, PhysicalDevice& m_physical_device, vk::SurfaceKHR& m_surface, GLFWwindow* m_window)
    : m_device(device), m_physical_device(m_physical_device), m_surface(m_surface), m_window(m_window)
{
    create_swapchain();
}

void Swapchain::destroy()
{
    destroy_swapchain();
}

void Swapchain::recreate_swapchain()
{
    // ensure we arent processing anything before recreating
    (*m_device).waitIdle();

    destroy_swapchain();

    create_swapchain();
}

void Swapchain::destroy_swapchain()
{
    for (size_t i = 0; i < m_swapchain_image_views.size(); i++) {
        (*m_device).destroyImageView(m_swapchain_image_views[i]);
    }

    (*m_device).destroySwapchainKHR(m_swapchain);
}

void Swapchain::create_swapchain()
{
    const SwapChainSupportDetails& swapChainSupport = m_physical_device.get_swapchain_support_details(m_surface);
    const QueueFamilyIndices& queueIndices = m_physical_device.get_queue_family_indices(m_surface);

    vk::SurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = choose_swap_present_mode(swapChainSupport.present_modes);
    vk::Extent2D extent = choose_swap_extent(swapChainSupport.capabilities);

    PBL_CORE_TRACE("Vulkan: Swap extent ({},{}).", extent.width, extent.height);

    // the number of images in the swap chain queue
    // request more than the min to reduce waits on driver
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // actually create the swap chain
    vk::SwapchainCreateInfoKHR createInfo(
        {},
        m_surface,
        imageCount,                 // min image count
        surfaceFormat.format,       // img format
        surfaceFormat.colorSpace,   // img color space
        extent,                     // img extent
        1                           // img array layers
    );
    
    uint32_t queue_family_indices[] = {queueIndices.graphics_family, queueIndices.presentation_family};
    if (queueIndices.graphics_family != queueIndices.presentation_family) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queue_family_indices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    m_swapchain = (*m_device).createSwapchainKHR(createInfo);

    m_images = (*m_device).getSwapchainImagesKHR(m_swapchain);

    m_image_format = surfaceFormat.format;
    m_extent = extent;

    create_views();
}

vk::SurfaceFormatKHR Swapchain::choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (const auto& format : availableFormats) { 
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR Swapchain::choose_swap_present_mode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& presentMode : availablePresentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Swapchain::choose_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        // if no special extents, return the m_surface extents
        return capabilities.currentExtent;
    } else {
        // calculate the best extents to use (closest to m_window size within limits)
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        vk::Extent2D actual_extent = {
            std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };

        return actual_extent;
    }
}

void Swapchain::create_views()
{
    m_swapchain_image_views.resize(m_images.size());

    // create an image view for each image in the swap chain
    for (size_t i = 0; i < m_images.size(); i++) {
        vk::ComponentMapping components(
            vk::ComponentSwizzle::eIdentity,    // r
            vk::ComponentSwizzle::eIdentity,    // g
            vk::ComponentSwizzle::eIdentity,    // b
            vk::ComponentSwizzle::eIdentity     // a
        );

        vk::ImageSubresourceRange subresourceRange(
            vk::ImageAspectFlagBits::eColor,    // aspect mask
            0,  // base mip lv
            1,  // lv count
            0,  // base array layer
            1   // layer count
        );

        vk::ImageViewCreateInfo createInfo(
            {},
            m_images[i],
            vk::ImageViewType::e2D,
            m_image_format,
            components,
            subresourceRange
        );
        
        m_swapchain_image_views[i] = (*m_device).createImageView(createInfo);
    }
}


}