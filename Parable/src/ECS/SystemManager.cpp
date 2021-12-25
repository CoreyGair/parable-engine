#include "SystemManager.h"

#include "ComponentManager.h"

namespace Parable::ECS
{
	
/**
 * Called when the ECS layer receives update (every frame).
 *
 * For now, a simple implementation which iterates over m_systems and calls all on_update() sequentially.
 * Calls systems in order of their m_order field.
 */ 
void SystemManager::on_update()
{
    for(ISystem& system : m_systems_by_order)
    {
        if (system.enabled) system.on_update();
    }
}

void SystemManager::set_enabled(SystemID s, bool enabled)
{
    m_systems_by_id[s]->enabled = enabled;
}


}