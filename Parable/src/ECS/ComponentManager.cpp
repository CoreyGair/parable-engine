#pragma once

#include "ComponentManager.h"

#include "Exception/MemoryExceptions.h"


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
																m_COMPONENT_CHUNK_SIZE,
																0,
																m_allocator.get_size() - m_allocator.get_used(),
																m_allocator.allocate(m_allocator.get_size() - m_allocator.get_used(), 0)
															)
{




}

ComponentManager::~ComponentManager()
{

	// deallocate space for the entity component lists
	m_allocator.deallocate(m_entity_component_list_allocator.get_start());

	// deallocate the component chunks
	m_allocator.deallocate(m_component_chunk_allocator.get_start());

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


ComponentManager::ComponentChunkManager::ComponentChunkManager(size_t component_size, size_t component_align, Allocator& chunk_allocator) :
																m_component_size(component_size),
																m_component_align(component_align)
																m_allocator(chunk_allocator);
																m_chunk_start(m_allocator.allocate(m_COMPONENT_CHUNK_SIZE,0))
{
	chunk = m_chunk_start;

	size_t max_components = m_COMPONENT_CHUNK_SIZE / m_component_size;

	m_num_flag_segments = max_components / (sizeof(FlagSegment) * 8);
	size_t avaliable_chunk_size = m_COMPONENT_CHUNK_SIZE;

	if (!std::align(alignof(FlagSegment), m_num_flag_segments * sizeof(FlagSegment), chunk, avaliable_chunk_size) == nullptr)
	{
		throw OutOfMemoryException("Not enough space in component chunk for flags!");
	}

	m_used_flags = chunk;
	chunk += m_num_flag_segments * sizeof(FlagSegment);
	for(int i = 0; i < m_num_flag_segments; ++i)
	{
		m_used_flags[i] = 0;
	}


	if (std::align(m_component_align, 0, chunk, avaliable_chunk_size) == nullptr)
	{
		throw OutOfMemoryException("Not enough space in component chunk for components!");
	}

	m_components = chunk;
	m_num_components = avaliable_chunk_size / m_component_size;
}


ComponentManager::ComponentChunkManager::~ComponentChunkManager()
{
	m_allocator.deallocate(m_chunk_start);
}

size_t ComponentManager::ComponentChunkManager::first_free_space()
{
	size_t x;
	const size_t segment_bitwidth = sizeof(FlagSegment) * 8;
	for(size_t i = 0; i < m_num_flag_segments; ++i)
    {
        if (m_segments[i] == 0)
		{
			x = i * segment_bitwidth;
			break;
		} 
        Segment s = ~m_segments[i];
        if (s != 0)
        {
            s = (s & ~(s-1));
            int pos = 0;
            while (s >>= 1) ++pos;
            x = i * segment_bitwidth + pos;
        }
    }
	return x;
}



}