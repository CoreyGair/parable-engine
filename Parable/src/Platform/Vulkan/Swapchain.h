#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"
#include "PhysicalDevice.h"

class GLFWwindow;


namespace Parable::Vulkan
{


/**
 * Wraps vk::Swapchain, images and image views.
 * 
 */
class Swapchain
{
public:
    Swapchain() = default;
    Swapchain(Device& device, PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, GLFWwindow* window);

    Swapchain(Swapchain&& other) = default;
    Swapchain& operator=(Swapchain&& other)
    {
        m_device = other.m_device;
        m_physical_device = other.m_physical_device;

        m_surface = other.m_surface;
        m_window = other.m_window;

        m_swapchain = std::move(other.m_swapchain);
        m_images = std::move(other.m_images);
        m_image_format = other.m_image_format;
        m_extent = other.m_extent;
        m_swapchain_image_views = std::move(other.m_swapchain_image_views);

        return *this;
    }

    void destroy();

    operator vk::SwapchainKHR&() { return m_swapchain; }
    vk::SwapchainKHR& operator*() { return m_swapchain; }

    void recreate_swapchain();

    vk::SwapchainKHR& get_swapchain() { return m_swapchain; }
    vk::Format& get_image_format() { return m_image_format; };
    vk::Extent2D& get_extent() { return m_extent; };

    const std::vector<vk::ImageView>& get_views() const { return m_swapchain_image_views; };

private:
    void destroy_swapchain();
    void create_swapchain();

    vk::SurfaceFormatKHR choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR choose_swap_present_mode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities);

    void create_views();

    /**
     * The device which owns/was used to create this swapchain.
     */
    Device m_device;

    /**
     * The physical device for which this swapchain works.
     */
    PhysicalDevice m_physical_device;

    /**
     * Where this swapchain presents to.
     */
    vk::SurfaceKHR m_surface;
    GLFWwindow* m_window;

    vk::SwapchainKHR m_swapchain;
    std::vector<vk::Image> m_images;
    vk::Format m_image_format;
    vk::Extent2D m_extent;

    std::vector<vk::ImageView> m_swapchain_image_views;
};


}