#pragma once

#include "../Loader/LoadTask.h"

#include "../Wrapper/Buffer.h"

namespace vk
{
class CommandBuffer;
class DescriptorPool;
}

namespace Parable
{
template<class ResourceType>
class ResourceStorageBlock;

class TextureLoadInfo;
class Texture;
}

namespace Parable::Vulkan
{


class Device;
class PhysicalDevice;

class TextureStateBlock;

class TextureLoadTask : public LoadTask
{
private:
    const TextureLoadInfo& m_load_info;

    ResourceStorageBlock<Parable::Texture>& m_texture_storage;

    /**
     * An allocated descriptor set for the new texture.
     */
    vk::DescriptorSet m_descriptor_set;

    Buffer m_staging_buffer;

public:
    TextureLoadTask(
        const TextureLoadInfo& load_info,
        ResourceStorageBlock<Parable::Texture>& texture_storage,
        vk::DescriptorSet descriptor_set
    )
        : m_load_info(load_info),
        m_texture_storage(texture_storage),
        m_descriptor_set(descriptor_set)
    {}
    
    void record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer) override;

    void on_load_complete() override;
};



}