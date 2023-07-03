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
class TextureLoadInfo;
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

    TextureStateBlock& m_texture_state;

    /**
     * An allocated descriptor set for the new texture.
     */
    vk::DescriptorSet m_descriptor_set;

    Buffer m_staging_buffer;

public:
    TextureLoadTask(
        const TextureLoadInfo& load_info,
        TextureStateBlock& texture_state,
        vk::DescriptorSet descriptor_set
    )
        : m_load_info(load_info),
        m_texture_state(texture_state),
        m_descriptor_set(descriptor_set)
    {}
    
    void record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer) override;

    void on_load_complete() override;
};



}