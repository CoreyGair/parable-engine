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
    int m_reference_count;
    AssetLoadState m_load_state;

public:
    ~AssetStateBlock()
    {
        // TODO: is this needed or desirable? also should there be an assertion on load_state??
        PBL_CORE_ASSERT(m_reference_count == 0);
    }

    AssetStateBlock& operator++()
    {
        m_reference_count++;
    }
    AssetStateBlock& operator--()
    {
        PBL_CORE_ASSERT(m_reference_count > 0);
        m_reference_count--;
    }

    int get_ref_count() const { return m_reference_count; }
    AssetLoadState get_load_state() const { return m_load_state; }
};


}
