#include "TextureStore.h"

#include <vulkan/vulkan.hpp>

#include "Asset/AssetRegistry.h"
#include "Asset/AssetLoadInfo.h"

#include "TextureLoadTask.h"
#include "Texture.h"

namespace Parable::Vulkan
{


TextureStore::TextureStore(Device device, vk::DescriptorSetLayout& texture_descriptor_set_layout, Loader& loader)
    : ResourceStore<Parable::Texture>(loader),
    m_device(device),
    m_descriptor_set_layout(texture_descriptor_set_layout)
{
    // for now, allocates just a large amt for dev
    vk::DescriptorPoolSize texture_descriptor_pool_sizes[] = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eCombinedImageSampler,
            // space for 100 samplers, so 100 textures with one each
            static_cast<uint32_t>(100)
        )
    };

    m_descriptor_pool = m_device->createDescriptorPool(vk::DescriptorPoolCreateInfo(
        {},
        // 100 descriptor sets containing one sampler each
        static_cast<uint32_t>(100),
        texture_descriptor_pool_sizes
    ));
}

std::unique_ptr<LoadTask> TextureStore::create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<Parable::Texture>& storage_block)
{
    const TextureLoadInfo& load_info = AssetRegistry::resolve<TextureLoadInfo>(descriptor);

    // allocate a descriptor for the new Texture
    vk::DescriptorSet descriptor_set = m_device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(
        m_descriptor_pool, 1, &m_descriptor_set_layout
    ))[0];

    return std::make_unique<TextureLoadTask>(load_info, storage_block, descriptor_set);
}


}
