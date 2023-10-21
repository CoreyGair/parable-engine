#pragma once

#include "pblpch.h"

#include "../Loader/LoadTask.h"

namespace Parable
{
template<class ResourceType>
class ResourceStorageBlock;

class EffectLoadInfo;
class Effect;
}

namespace Parable::Vulkan
{


/**
 * A EffectLoadTask represents the task of loading an effect (compiling pipelines/shaders etc.)
 */
class EffectLoadTask : public LoadTask
{
private:
    const EffectLoadInfo& m_load_info;

    Parable::ResourceStorageBlock<Parable::Effect>& m_effect_storage;

public:
    EffectLoadTask(
        const EffectLoadInfo& load_info,
        Parable::ResourceStorageBlock<Parable::Effect>& effect_storage
    );

    /**
     * Create pipelines and other state objects.
    */
    void create_device_resources(Device& device) override;

    // unused
    void record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer) override {}
    void on_load_complete() override {}
};


}

