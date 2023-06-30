#include "Texture.h"

namespace Parable::Vulkan
{


    // // CREATE texture image view
    // m_image_view = image.m_device->createImageView(
    //     vk::ImageViewCreateInfo(
    //         {},
    //         m_image,
    //         vk::ImageViewType::e2D,
    //         vk::Format::eR8G8B8A8Srgb,
    //         {},
    //         vk::ImageSubresourceRange(
    //             vk::ImageAspectFlagBits::eColor,
    //             0, // baseMipLevel
    //             1, // levelCount
    //             0, // baseArrayLayer
    //             1  // layerCount
    //         )
    //     )
    // );

    // // CREATE texture image sampler
    // m_sampler = m_device->createSampler(
    //     vk::SamplerCreateInfo(
    //         {},
    //         vk::Filter::eLinear,    // magFilter
    //         vk::Filter::eLinear,    // minFilter
    //         vk::SamplerMipmapMode::eLinear, // mipmapMode
    //         vk::SamplerAddressMode::eRepeat,    // addressModeU
    //         vk::SamplerAddressMode::eRepeat,    // addressModeV
    //         vk::SamplerAddressMode::eRepeat,    // addressModeW
    //         0.0f,   // mipLodBias
    //         true,   // anisotropyEnable
    //         m_physical_device->getProperties().limits.maxSamplerAnisotropy, // maxAnisotropy,
    //         false,  // compareEnable
    //         vk::CompareOp::eAlways,
    //         0.0f,   // minLod
    //         0.0f,   // maxLod
    //         vk::BorderColor::eIntOpaqueBlack,   // borderColor
    //         false   // unnormalizedCoordinates
    //     )
    // );

    // vk::DescriptorImageInfo image_info(
    //     sampler,
    //     view,
    //     vk::ImageLayout::eShaderReadOnlyOptimal
    // );

    // vk::WriteDescriptorSet descriptor_writes[] = {
    //     vk::WriteDescriptorSet(
    //         m_descriptor_set,
    //         0, // binding
    //         0, // arrayElement
    //         1, // descriptorCount
    //         vk::DescriptorType::eCombinedImageSampler,
    //         &image_info,            // pImageInfo
    //         {},                     // pBufferInfo
    //         {}                      // pTexelBufferView
    //     )
    // };

    // m_device->updateDescriptorSets(descriptor_writes, {});


}
