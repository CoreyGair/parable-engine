#pragma once

#include "ComponentManager.h"


namespace Parable::ECS
{


ComponentManager::ComponentManager(Allocator& allocator) : m_allocator(allocator),						
															m_entity_component_list_allocator(				// This is ugly but has to be done here
																PoolAllocator::create<IComponent*>(
																	m_ENTITY_COMPONENT_LISTS_ALLOC_SIZE,
																	m_allocator.allocate(m_ENTITY_COMPONENT_LISTS_ALLOC_SIZE, alignof(IComponent*))
																)
															), 
															m_component_chunk_allocator(
																sizeof(ComponentChunk) + m_COMPONENT_CHUNK_SIZE,
																alignof(ComponentChunk),
																m_allocator.get_size() - m_allocator.get_used(),
																m_allocator.allocate(m_allocator.get_size() - m_allocator.get_used(), 0)
															)
{




}

ComponentManager::~ComponentManager()
{

// deallocate space for the entity component lists
m_allocator.deallocate(m_entity_component_list_allocator.get_start());

}

void ComponentManager::Init()
{
	m_is_initialised = true;
}

void ComponentManager::add_entity(Entity e)
{


	// initialise entity component list
	IComponent** component_ptrs = m_entity_component_list_allocator.allocate_array<IComponent*>(m_registered_components);
	if (e < m_entity_component_lists.size())
	{
		// already have a space in the vector for this entity
		m_entity_component_lists[e] = component_ptrs;
	}
	else 
	{
		// must be e == m_entity_component_lists.size(), so just add an extra element
		m_entity_component_lists.emplace_back(component_ptrs);
	}
}

void ComponentManager::remove_entity(Entity e)
{
	// clean up entity component list
	m_entity_component_list_allocator.deallocate_array<IComponent*>(m_entity_component_lists[e]);
	if (e+1 == m_entity_component_lists.size())
	{
		// is end of array, can remove
		m_entity_component_lists.pop_back();
	}
	else
	{
		m_entity_component_lists[e] = nullptr;
	}
}

void ComponentManager::add_component(Entity e, ComponentTypeID c)
{
	m_component_chunk_lists[c]
}
void ComponentManager::remove_component(Entity e, ComponentTypeID c)
{

}

ComponentManager::ComponentChunk::ComponentChunk(size_t chunk_size, size_t component_size, size_t component_align) : m_component_size(component_size)
{
	// seek a pointer to the end of this class:
	// the ComponentManager will allocate chunk_size extra space here to store the components
	void* components_space = this + sizeof(ComponentManager::ComponentChunk);

	// align this address to the desired component align
	std::align(component_align, 0, components_space, chunk_size);

	m_components = (IComponent*)components_space; 

	// calculate number of components we can fit in the chunk
	m_component_count = chunk_size / component_size;
}


}