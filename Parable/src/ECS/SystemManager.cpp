

#include "SystemManager.h"


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
    for(UPtr<ISystem>& system : m_systems)
    {
        if (system->enabled) system->on_update();
    }
}


}