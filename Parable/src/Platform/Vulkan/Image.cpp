#include "Image.h"

#include "GPU.h"

#include "VulkanExceptions.h"

namespace Parable::Vulkan
{


Image::Image(GPU& gpu, VkImageCreateInfo& image_info)
    : m_gpu(gpu)
{
    VkResult result = vkCreateImage(m_gpu.device, &image_info, nullptr, &m_image);

    if (result != VK_SUCCESS)
    {
        throw VulkanFailedCreateException("image", result);
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(m_gpu.device, m_image, &mem_requirements);

    m_image_memory = m_gpu.allocate_device_memory(mem_requirements.size, gpu.pick_memory_type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    vkBindImageMemory(m_gpu.device, m_image, m_image_memory, 0);
}

Image::~Image()
{

}


}