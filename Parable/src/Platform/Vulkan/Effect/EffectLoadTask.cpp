#include "EffectLoadTask.h"

#include <vulkan/vulkan.hpp>

#include "EffectData.h"
#include "Effect.h"

#include "Asset/ResourceState.h"
#include "Asset/AssetLoadInfo.h"

namespace Parable::Vulkan
{


EffectLoadTask::EffectLoadTask(
    const EffectLoadInfo& load_info,
    Parable::ResourceStorageBlock<Parable::Effect>& effect_storage
)
    : m_load_info(load_info),
    m_effect_storage(effect_storage)
{}

void EffectLoadTask::create_device_resources(Device& device)
{
    EffectData effect_data(m_load_info, device);

    m_effect_storage.set_resource(std::make_unique<Effect>(??));

    m_effect_storage.set_load_state(Parable::ResourceLoadState::Loaded);
}


}
