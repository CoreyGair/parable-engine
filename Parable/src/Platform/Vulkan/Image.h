#pragma once

#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "Buffer.h"


namespace Parable::Vulkan
{


class PhysicalDevice;

class Image
{
public:
    Image() = default;
    Image(Image&& other) = default;
    Image(Device& device, PhysicalDevice& physicalDevice, vk::ImageCreateInfo& imageInfo);
    
    Image& operator=(Image&& other)
    {
        m_device = other.m_device;

        m_image = other.m_image;
        m_image_memory = other.m_image_memory;

        return *this;
    }

    operator vk::Image&() { return m_image; }

    void destroy()
    {
        (*m_device).destroyImage(m_image);
        m_device.free_memory(m_image_memory);
    }

    void transition_layout(vk::CommandBuffer& cmdBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        vk::ImageMemoryBarrier barriers[1] = {vk::ImageMemoryBarrier(
            {},{},
            oldLayout,newLayout,
            VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,
            m_image,
            vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor,
                0, // baseMipLevel
                1, // levelCount
                0, // baseArrayLayer
                1  // layerCount
            )
        )};

        // this bit is from tutorial, need to set access masks
        // might be a better more inclusive way of this
        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;
        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
            barriers[0].srcAccessMask = {};
            barriers[0].dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barriers[0].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barriers[0].dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        cmdBuffer.pipelineBarrier(
            sourceStage, destinationStage,
            {},
            {},
            {},
            barriers
        );
    }

    void copy_from_buffer(vk::CommandBuffer& cmdBuffer, Buffer& srcBuffer, uint32_t width, uint32_t height)
    {
        vk::BufferImageCopy regions[1] = {
            vk::BufferImageCopy(
                0, // offset
                0, // bufferRowLength
                0, // bufferImageHeight
                vk::ImageSubresourceLayers(
                    vk::ImageAspectFlagBits::eColor,
                    0, // mipLevel
                    0, // baseArrayLayer
                    1  // layerCount
                ),
                {0,0,0},            // imageOffset
                {width,height,1}    // imageExtent
            )
        };

        // assume the image is in eTransferDstOptimal, if not this will break
        cmdBuffer.copyBufferToImage(srcBuffer, m_image, vk::ImageLayout::eTransferDstOptimal, regions);
    }

private:
    Device m_device;

    vk::Image m_image;
    vk::DeviceMemory m_image_memory;
};

class RecreatableImage
{
public:
    RecreatableImage() = default;
    RecreatableImage(Device& device, PhysicalDevice& physicalDevice, vk::ImageCreateInfo& imageInfo)
        : info(imageInfo), m_image(device, physicalDevice, imageInfo)
    {}

    operator vk::Image&() { return m_image; }
    operator Image&() { return m_image; }

    void destroy()
    {
        m_image.destroy();
    }

    void recreate(Device& device, PhysicalDevice& physicalDevice)
    {
        destroy();
        m_image = Image(device, physicalDevice, info);
    }
    
    vk::ImageCreateInfo info;

private:
    Image m_image;
};

}
