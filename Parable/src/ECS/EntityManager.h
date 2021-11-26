#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Entity.h"

namespace Parable::ECS
{


/*
 * Manages allocation of entity id's within the ECS.
 */
class EntityManager
{
public:
    EntityManager() = default;
    ~EntityManager() = default;

    Entity create();
    void destroy(Entity e);

private:
    /*
     * The next entity ID to be allocated.
     *
     * Default allocation is in order (0,1,2,...)
     */
    Entity m_next_entity = 0;

    /*
     * Queues deallocated entity ID's for reuse.
     *
     * These ID's are used before coninuing from m_next_entity.
     */
    std::queue<Entity> m_toombstone_entities;
};


}