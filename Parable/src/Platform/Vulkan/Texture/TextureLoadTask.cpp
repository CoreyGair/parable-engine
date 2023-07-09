#include "TextureLoadTask.h"

#include "pblpch.h"

#include "Core/Base.h"

#include <vulkan/vulkan.hpp>

#include "../Wrapper/Buffer.h"
#include "../Wrapper/Image.h"

#include "TextureData.h"
#include "TextureStateBlock.h"
#include "Texture.h"

#include "Asset/AssetLoadInfo.h"

namespace Parable::Vulkan
{


void TextureLoadTask::record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer)
{
    TextureData data = TextureData::from_png(m_load_info.get_png_path());

    // set up staging buf
    BufferBuilder staging_buffer_builder;
    staging_buffer_builder.buffer_info.size = data.get_pixels_size();
    staging_buffer_builder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    staging_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    staging_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    m_staging_buffer = staging_buffer_builder.create(device, physical_device);

    // write tex to staging
    m_staging_buffer.write(data.get_pixels(), 0, data.get_pixels_size());

    // create image
    vk::ImageCreateInfo image_info(
        {},
        vk::ImageType::e2D,
        vk::Format::eR8G8B8A8Srgb,
        vk::Extent3D(data.get_dimensions().width, data.get_dimensions().height, 1),
        1, // mipLevels
        1, // arrayLevels
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::SharingMode::eExclusive,
        {},
        vk::ImageLayout::eUndefined
    );

    Image texture_image(device, physical_device, image_info);

    // now record commands

    // first transition to transfer dest opt
    vk::ImageMemoryBarrier barriers = {vk::ImageMemoryBarrier(
        {},
        vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,
        texture_image,
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor,
            0, // baseMipLevel
            1, // levelCount
            0, // baseArrayLayer
            1  // layerCount
        )
    )};

    command_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barriers
    );

    texture_image.copy_from_buffer(command_buffer, m_staging_buffer, data.get_dimensions().width, data.get_dimensions().height);

    // now can transition texture to shader optimal
    barriers = {vk::ImageMemoryBarrier(
        vk::AccessFlagBits::eTransferWrite,
        {},
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,
        texture_image,
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor,
            0, // baseMipLevel
            1, // levelCount
            0, // baseArrayLayer
            1  // layerCount
        )
    )};

    command_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        {},
        {},
        {},
        barriers
    );

    // CREATE texture image view
    vk::ImageView texture_view = device->createImageView(
        vk::ImageViewCreateInfo(
            {},
            texture_image,
            vk::ImageViewType::e2D,
            vk::Format::eR8G8B8A8Srgb,
            {},
            vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor,
                0, // baseMipLevel
                1, // levelCount
                0, // baseArrayLayer
                1  // layerCount
            )
        )
    );

    // CREATE texture image sampler
    vk::Sampler texture_sampler = device->createSampler(
        vk::SamplerCreateInfo(
            {},
            vk::Filter::eLinear,    // magFilter
            vk::Filter::eLinear,    // minFilter
            vk::SamplerMipmapMode::eLinear, // mipmapMode
            vk::SamplerAddressMode::eRepeat,    // addressModeU
            vk::SamplerAddressMode::eRepeat,    // addressModeV
            vk::SamplerAddressMode::eRepeat,    // addressModeW
            0.0f,   // mipLodBias
            true,   // anisotropyEnable
            physical_device->getProperties().limits.maxSamplerAnisotropy, // maxAnisotropy,
            false,  // compareEnable
            vk::CompareOp::eAlways,
            0.0f,   // minLod
            0.0f,   // maxLod
            vk::BorderColor::eIntOpaqueBlack,   // borderColor
            false   // unnormalizedCoordinates
        )
    );

    // write to descriptor set

    vk::DescriptorImageInfo texture_combined_sampler_info(
        texture_sampler,
        texture_view,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    vk::WriteDescriptorSet descriptor_writes[] = {
        vk::WriteDescriptorSet(
            m_descriptor_set,
            0, // binding
            0, // arrayElement
            1, // descriptorCount
            vk::DescriptorType::eCombinedImageSampler,
            &texture_combined_sampler_info, // pImageInfo
            {},                     // pBufferInfo
            {}                      // pTexelBufferView
        )
    };

    device->updateDescriptorSets(descriptor_writes, {});

    // now we can construct the Texture object and place it into the state block
    m_texture_state.set_texture(Texture(std::move(texture_image), texture_view, texture_sampler, m_descriptor_set));
}

void TextureLoadTask::on_load_complete()
{
    m_staging_buffer.destroy();

    m_texture_state.set_load_state(AssetLoadState::Loaded);
}


}