#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include <vulkan/vulkan.h>
#include "../Wrapper/Device.h"

#include "../ResourceStore.h"

#include "Asset/AssetDescriptor.h"

namespace Parable
{
class Texture;
}

namespace Parable::Vulkan
{


class Loader;

/**
 * Handles the storage and lookup of Vulkan Texture resources.
 */
class TextureStore : public ResourceStore<Parable::Texture>
{
private:
    Device m_device;

    vk::DescriptorSetLayout& m_descriptor_set_layout;

    vk::DescriptorPool m_descriptor_pool;

public:
    TextureStore(Device device, vk::DescriptorSetLayout& texture_descriptor_set_layout, Loader& loader);

    ~TextureStore()
    {
        m_device->destroyDescriptorPool(m_descriptor_pool);
    }   

    std::unique_ptr<LoadTask> create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<Parable::Texture>& storage_block) override;
};


}
