#include "Core/Base.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>

namespace Parable
{


void create_vulkan_instance(VkInstance* instance) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Parable";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
    VkResult result = (pfnCreateInstance)(&createInfo, nullptr, instance);

    if (result != VK_SUCCESS) {
        PBL_CORE_ERROR("Failed to create Vulkan instance, error: %i", result);
    }
}

void destroy_vulkan_instance(VkInstance instance)
{
    PFN_vkDestroyInstance pfnDestroyInstance = (PFN_vkDestroyInstance)glfwGetInstanceProcAddress(instance, "vkDestroyInstance");
    (pfnDestroyInstance)(instance, nullptr);
}


}