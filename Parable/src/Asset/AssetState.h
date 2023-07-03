#pragma once

#include "Core/Base.h"

namespace Parable
{


enum class AssetLoadState
{
    Unloaded,
    Loading,
    Loaded
};

class AssetStateBlock
{
private:
    int m_reference_count = 0;
    AssetLoadState m_load_state = AssetLoadState::Unloaded;

public:
    ~AssetStateBlock()
    {
        // TODO: is this needed or desirable? also should there be an assertion on load_state??
        PBL_CORE_ASSERT(m_reference_count == 0);
    }

    AssetStateBlock& operator++()
    {
        m_reference_count++;
        return *this;
    }
    AssetStateBlock& operator--()
    {
        PBL_CORE_ASSERT(m_reference_count > 0);
        m_reference_count--;
        return *this;
    }

    int get_ref_count() const { return m_reference_count; }
    AssetLoadState get_load_state() const { return m_load_state; }
    void set_load_state(AssetLoadState state) { m_load_state = state; }
};


}
