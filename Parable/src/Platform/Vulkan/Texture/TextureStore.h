#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include <vulkan/vulkan.h>
#include "../Wrapper/Device.h"

#include "Renderer/Handles.h"

#include "Asset/AssetDescriptor.h"

#include "TextureStateBlock.h"

namespace Parable::Vulkan
{


class Loader;

/**
 * Handles the storage and lookup of Vulkan Texture resources.
 */
class TextureStore
{
private:
    Loader& m_loader;

    std::map<AssetDescriptor,TextureStateBlock> m_state_blocks;

    Device m_device;

    vk::DescriptorSetLayout& m_descriptor_set_layout;

    vk::DescriptorPool m_descriptor_pool;

public:
    TextureStore(Device device, vk::DescriptorSetLayout& texture_descriptor_set_layout, Loader& loader);

    ~TextureStore()
    {
        m_device->destroyDescriptorPool(m_descriptor_pool);
    }

    TextureHandle load(AssetDescriptor descriptor);

    Texture& get_texture(Parable::TextureHandle& handle);
};


}
