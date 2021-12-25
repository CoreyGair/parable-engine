#include "EntityManager.h"

#include "ComponentManager.h"
#include "EntityComponentMap.h"

namespace Parable::ECS
{


/**
 * Allocates a new Entity ID, creating a new entity.
 *
 * Takes from the front of m_toombstone_entities if avaliable, or increments m_next_entity if not.
 *
 * @return the allocated Entity.
 */
Entity EntityManager::create()
{
    Entity e;

    // either pick the next unused entity or use a previously destroyed one.
    if(m_toombstone_entities.empty())
    {
        e = m_next_entity++;
    }
    else
    {
        e = m_toombstone_entities.front();
        m_toombstone_entities.pop();
    }

    return e;
}


/**
 * Destroy an entity, and enqueue its ID for reuse.
 */
void EntityManager::destroy(Entity e)
{
    m_toombstone_entities.push(e);
}


}