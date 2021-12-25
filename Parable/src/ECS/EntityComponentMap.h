#pragma once

#include "Core/Base.h"

#include "Entity.h"
#include "Component.h"


namespace Parable
{
	class Allocator;
	class PoolAllocator;
}

namespace Parable::ECS
{


class EntityComponentMap
{
public:
	EntityComponentMap(ComponentTypeID num_components, size_t allocation_size, Allocator& parent_allocator);
	~EntityComponentMap();

	void add_entity(Entity e);
	void remove_entity(Entity e);

	IComponent **& operator[](size_t i) { return m_entity_component_lists[i]; }

	std::vector<IComponent**>& get_map() { return m_entity_component_lists; }

private:
	/**
	 * The parent allocator, used to allocate space for the component list pool.
	 */
	Allocator& m_parent_allocator;

	// TODO: maybe make this a new 'ExpandableAllocator'
	// atm, pool allocs fixed size arrays of IComponent*
	// could make expandable by allowing it to malloc() a chunk of N (like 100) Icomponent* arrays,
	// as this currently limits the number of entitites we can handle to the ammount prealloced at the start
	/**
	 * Allocates space for the entity component lists.
	 */
	UPtr<PoolAllocator> m_allocator;

	/**
	 * Stores lists of pointers to components for each currently alive entity.
	 * 
	 * m_entity_component_lists[e] is an array of Component* attached to the Entity e.
	 * m_entity_component_lists[e][c] is the Component* for the ComponentTypeID c attached to the Entity e.
	 */
	std::vector<IComponent**> m_entity_component_lists;
};


}