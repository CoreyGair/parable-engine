#include "EntityComponentMap.h"

#include "Component.h"

#include "Memory/Allocator.h"
#include "Memory/PoolAllocator.h"

#include "Exception/MemoryExceptions.h"

namespace Parable::ECS
{


/**
 * Construct a new EntityComponentMap object.
 * 
 * @param num_components the number of different component types to be stored in this map.
 * @param allocation_size the total ammount of memory allocated to this map.
 * @param parent_allocator the allocator to allocate memory for the map from.
 */
EntityComponentMap::EntityComponentMap(ComponentTypeID num_components, size_t allocation_size, Allocator& parent_allocator) :
																		m_parent_allocator(parent_allocator)
{
	PBL_CORE_ASSERT_MSG(num_components > 0, "Cannot create entity component map with 0 components!");
	m_allocator = std::make_unique<PoolAllocator>(sizeof(IComponent*) * num_components,
													alignof(IComponent*),
													allocation_size,
													m_parent_allocator.allocate(allocation_size, alignof(IComponent*))
												);						
}

EntityComponentMap::~EntityComponentMap()
{
	// dealloc all remaining entity component lists
	for(IComponent** i : m_entity_component_lists)
	{
		if (i == nullptr) continue;

		m_allocator->deallocate(i);
	}

	// return memory to the parent alloc
	m_parent_allocator.deallocate(m_allocator->get_start());
}

/**
 * Add an entity to the mapping.
 * 
 * Allocates a list of component pointers to store the components attached to this entity.
 * 
 * @param e the entity to add to the map.
 */
void EntityComponentMap::add_entity(Entity e)
{
	// check the entity hasnt yet been added, or was previously removed
	if (e < m_entity_component_lists.size() && m_entity_component_lists[e] != nullptr) return;

	// initialise entity component list
	IComponent** component_ptrs = (IComponent**)m_allocator->allocate(m_allocator->get_object_size(), alignof(IComponent*));
	if (component_ptrs == nullptr)
	{
		throw OutOfMemoryException("Ran out of memory to store entity component lists!");
	}

	// set all to nullptrs (entity starts with no components)
	for(size_t i = 0, space = m_allocator->get_object_size(); space > 0; ++i, space -= sizeof(IComponent*))
	{
		component_ptrs[i] = nullptr;
	}

	if (e < m_entity_component_lists.size())
	{
		// already have a space in the vector for this entity
		m_entity_component_lists[e] = component_ptrs;
	}
	else 
	{
		PBL_CORE_ASSERT_MSG(e == m_entity_component_lists.size(), "Entity {} out of range for add_entity (current size = {})!", e, m_entity_component_lists.size());

		// must be e == m_entity_component_lists.size(), so just add an extra element
		m_entity_component_lists.emplace_back(component_ptrs);
	}
}

/**
 * Remove an entity from the mapping.
 * 
 * Allocates a list of component pointers to store the components attached to this entity.
 * 
 * @param e the entity to add to the map.
 */
void EntityComponentMap::remove_entity(Entity e)
{
	// has the entity been previously added and has it not been removed yet?
	if (e >= m_entity_component_lists.size() || m_entity_component_lists[e] == nullptr) return;

	// clean up entity component list
	m_allocator->deallocate(m_entity_component_lists[e]);
	m_entity_component_lists[e] = nullptr;
}


}