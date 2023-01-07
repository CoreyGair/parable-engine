#include "Image.h"

#include "VulkanExceptions.h"

#include "PhysicalDevice.h"

namespace Parable::Vulkan
{


/**
 * @brief Constructs a vk::Image object, and allocates some device local memory for it.
 * 
 * @param device 
 * @param physicalDevice
 * @param imageInfo 
 */
Image::Image(Device& device, PhysicalDevice& physicalDevice, vk::ImageCreateInfo& imageInfo)
    : m_device(device)
{
    m_image = (*m_device).createImage(imageInfo);

    VkMemoryRequirements memRequirements = (*m_device).getImageMemoryRequirements(m_image);

    m_image_memory = m_device.allocate_memory(memRequirements.size, physicalDevice.pick_memory_type(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    (*m_device).bindImageMemory(m_image, m_image_memory, 0);
}


}