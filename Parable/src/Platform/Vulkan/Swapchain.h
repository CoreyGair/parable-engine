#pragma once


#include <vulkan/vulkan.h>


class GLFWwindow;

namespace Parable::Vulkan
{


class GPU;

class Swapchain
{
public:
    Swapchain(GPU& gpu, GLFWwindow* window);
    ~Swapchain();

    void recreate_swapchain(GLFWwindow* window);

    VkSwapchainKHR get_swapchain() const { return m_swapchain; }
    VkFormat get_image_format() const { return m_image_format; };
    VkExtent2D get_extent() const { return m_extent; };

    const std::vector<VkImageView>& get_views() const { return m_swapchain_image_views; };

private:
    void destroy_swapchain();
    void create_swapchain(GLFWwindow* window);

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkExtent2D choose_swap_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

    void create_views();

    /**
     * The GPU which owns/was used to create this swapchain.
     */
    GPU& m_gpu;

    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    VkFormat m_image_format;
    VkExtent2D m_extent;

    std::vector<VkImageView> m_swapchain_image_views;
};


}