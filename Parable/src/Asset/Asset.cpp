#include "pblpch.h"

#include "Core/Base.h"

#include "Asset.h"


namespace Parable
{


Asset::~Asset()
{
    PBL_CORE_ASSERT(m_references == 0);
    
    // TODO: could this just unload the asset if not already unloaded
    // may be complicated if in loading state tho...
    PBL_CORE_ASSERT(m_state == AssetState::Unloaded)
}


}