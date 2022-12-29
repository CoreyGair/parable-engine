#pragma once

#include <vulkan/vulkan.h>


namespace Parable::Vulkan
{
    

class GPU;

class Image
{
public:
    Image(GPU& gpu, VkImageCreateInfo& image_info);
    ~Image();

private:
    GPU& m_gpu;

    VkImage m_image;
    VkDeviceMemory m_image_memory;
};


}
