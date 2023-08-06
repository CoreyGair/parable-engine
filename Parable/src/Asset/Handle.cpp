#include "Handle.h"

#include "Core/Base.h"

#include "ResourceState.h"

namespace Parable
{


StateHandle::StateHandle(ResourceState& state_block) : m_state_block(&state_block)
{
    m_state_block->increment_ref_count();
}

StateHandle::~StateHandle()
{
    PBL_CORE_ASSERT(m_state_block->get_ref_count() > 0)
    if (m_state_block)
    {
        m_state_block->decrement_ref_count();
    }
}

StateHandle::StateHandle(const StateHandle& other)
{
    if (other.m_state_block)
    {
        m_state_block = other.m_state_block;
        m_state_block->increment_ref_count();
    }
}

StateHandle& StateHandle::operator=(const StateHandle& other)
{
    if (other.m_state_block)
    {
        m_state_block = other.m_state_block;
        m_state_block->increment_ref_count();
    }
    return *this;
}

StateHandle::StateHandle(StateHandle&& other)
{
    if (other.m_state_block)
    {
        m_state_block = other.m_state_block;
        other.m_state_block = nullptr;
    }
}

StateHandle& StateHandle::operator=(StateHandle&& other)
{
    if (other.m_state_block)
    {
        m_state_block = other.m_state_block;
        other.m_state_block = nullptr;
    }
    return *this;
}

bool StateHandle::is_loaded() const
{
    return m_state_block && m_state_block->get_load_state() == ResourceLoadState::Loaded;
}


}
