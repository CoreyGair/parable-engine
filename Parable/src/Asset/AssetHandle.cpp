#include "AssetHandle.h"

#include "AssetState.h"

namespace Parable
{


AssetHandle::AssetHandle(AssetStateBlock* state_block) : m_state_block(state_block)
{
    if (m_state_block)
    {
        ++(*m_state_block);
    }
}

AssetHandle::~AssetHandle()
{
    if (m_state_block)
    {
        --(*m_state_block);
    }
}

bool AssetHandle::is_loaded() const { return m_state_block->get_load_state() == AssetLoadState::Loaded; }


}
