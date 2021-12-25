#include "ECS.h"

#include "Entity.h"
#include "Component.h"
#include "System.h"

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "EntityComponentMap.h"

#include "Memory/LinearAllocator.h"

namespace Parable::ECS
{


ECS::ECSBuilder::ECSBuilder() : m_component_registry(std::make_unique<ComponentRegistry>()) {}

/**
 * Creates a new ECS object from the set values.
 * 
 * Constructs the EntityManager, ComponentManager, SystemManager and EntityComponentMap, as well as allocating the specified ammount of memory.
 */
UPtr<ECS> ECS::ECSBuilder::create()
{
	PBL_CORE_ASSERT_MSG(!created, "Cannot create() from the same ECSBuilder!");

	size_t total_size = m_entity_component_map_size + m_component_chunks_total_size;

	UPtr<Allocator> allocator = std::make_unique<LinearAllocator>(total_size, malloc(total_size));

	UPtr<EntityManager> entity_manager = std::make_unique<EntityManager>();

	UPtr<ComponentManager> component_manager = std::make_unique<ComponentManager>(*m_component_registry, m_component_chunks_total_size, m_component_chunk_size, m_entity_component_map_size, *allocator);

	UPtr<SystemManager> system_manager = std::make_unique<SystemManager>();

	return UPtr<ECS>(new ECS(std::move(entity_manager), std::move(component_manager), std::move(system_manager), std::move(allocator)));
}

/**
 * Construct a new ECS.
 * 
 * Takes control of a bunch of ptrs to implementations created by ECSBuilder.
 */
ECS::ECS(UPtr<EntityManager> entity_manager,
			UPtr<ComponentManager> component_manager,
			UPtr<SystemManager> system_manager,
			UPtr<Allocator> allocator) :
												m_entity_manager(std::move(entity_manager)),
												m_component_manager(std::move(component_manager)),
												m_system_manager(std::move(system_manager)),
												m_allocator(std::move(allocator))
{

}


// ECS IMPLEMENTATION

void ECS::on_update()
{

}

Entity ECS::create_entity()
{
	Entity e = m_entity_manager->create();

	m_component_manager->add_entity(e);

	return e;
}

void ECS::destroy_entity(Entity e)
{
	m_component_manager->remove_entity(e);

	m_entity_manager->destroy(e);
}


}