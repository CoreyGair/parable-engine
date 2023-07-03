#include "TextureStore.h"

#include <vulkan/vulkan.hpp>

#include "Asset/AssetRegistry.h"
#include "Asset/AssetLoadInfo.h"

#include "../Loader/Loader.h"
#include "TextureStateBlock.h"
#include "TextureLoadTask.h"
#include "Texture.h"

namespace Parable::Vulkan
{


TextureStore::TextureStore(Device device, vk::DescriptorSetLayout& texture_descriptor_set_layout, Loader& loader)
    : m_device(device),
    m_descriptor_set_layout(texture_descriptor_set_layout),
    m_loader(loader)
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

TextureHandle TextureStore::load(AssetDescriptor descriptor)
{
    auto hint = m_state_blocks.lower_bound(descriptor);
    if (hint != m_state_blocks.end() && hint->first == descriptor)
    {
        return TextureHandle(&hint->second);
    }

    const TextureLoadInfo& load_info = AssetRegistry::resolve<TextureLoadInfo>(descriptor);
    
    // its state block goes into the Loading state until the mesh data is uploaded to GPU buffers
    auto it = m_state_blocks.emplace_hint(hint, descriptor, TextureStateBlock(Texture()));
    it->second.set_load_state(AssetLoadState::Loading);

    // allocate a descriptor for the new Texture
    vk::DescriptorSet descriptor_set = m_device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(
        m_descriptor_pool, 1, &m_descriptor_set_layout
    ))[0];

    // now we must submit a load task to copy the mesh data to gpu buffers
    std::unique_ptr<TextureLoadTask> load_task = std::make_unique<TextureLoadTask>(load_info, it->second, descriptor_set);
    m_loader.submit_task(std::move(load_task));

    return TextureHandle(&it->second);
}

Texture& TextureStore::get_texture(TextureHandle& handle)
{
    if (!handle)
    {
        throw std::runtime_error("get_mesh: null handle dereference.");
    }

    TextureStateBlock* state_block = static_cast<TextureStateBlock*>(handle.get_state_block());

    return state_block->get_texture();
}


}
