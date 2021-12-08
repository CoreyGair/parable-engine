#include "ComponentManager.h"

#include "Exception/MemoryExceptions.h"

#include "Util/Pointer.h"


namespace Parable::ECS
{


//TODO: these are temp until static ecs manages them
size_t ComponentManager::ENTITY_COMPONENT_LISTS_SIZE;
size_t ComponentManager::COMPONENT_CHUNK_SIZE;

ComponentManager::ComponentManager(size_t entity_component_lists_size, size_t component_chunk_size, Allocator& allocator) :
															m_allocator(allocator),			
															m_entity_component_list_allocator(				// This is ugly but has to be done here
																sizeof(IComponent*),
																alignof(IComponent*),
																entity_component_lists_size,
																m_allocator.allocate(entity_component_lists_size, alignof(IComponent*))
															), 
															m_component_chunk_allocator(
																component_chunk_size,
																0,
																m_allocator.get_size() - m_allocator.get_used(),
																m_allocator.allocate(m_allocator.get_size() - m_allocator.get_used(), 0)
															)
{

	ENTITY_COMPONENT_LISTS_SIZE = entity_component_lists_size;
	COMPONENT_CHUNK_SIZE = component_chunk_size;

}

ComponentManager::~ComponentManager()
{
	// deallocate space for the entity component lists
	for(size_t i = 0; i < m_entity_component_lists.size(); ++i)
	{
		remove_entity(i);
	}
	m_allocator.deallocate(m_entity_component_list_allocator.get_start());

	// deallocate the component chunks
	m_allocator.deallocate(m_component_chunk_allocator.get_start());

}

void ComponentManager::init()
{
	m_is_initialised = true;

	// can save mem as these no longer change
	m_component_sizes.shrink_to_fit();
	m_component_aligns.shrink_to_fit();

	// set up entity component list allocation
	m_entity_component_list_allocator = std::move(PoolAllocator(sizeof(IComponent*) * m_registered_components,
												alignof(IComponent*),
												ENTITY_COMPONENT_LISTS_SIZE,
												m_entity_component_list_allocator.get_start()
												));

	// construct all component managers
	for (ComponentTypeID i = 0; i < m_registered_components; ++i)
	{
		m_chunk_managers.emplace_back(m_component_sizes[i], m_component_aligns[i], m_component_chunk_allocator);
	}
	m_chunk_managers.shrink_to_fit();
}

void ComponentManager::add_entity(Entity e)
{
	// initialise entity component list
	IComponent** component_ptrs = (IComponent**)m_entity_component_list_allocator.allocate(sizeof(IComponent*) * m_registered_components, alignof(IComponent*));
	if (component_ptrs == nullptr)
	{
		throw OutOfMemoryException("Ran out of memory to store entity component lists!");
	}

	if (e < m_entity_component_lists.size())
	{
		// already have a space in the vector for this entity
		m_entity_component_lists[e] = component_ptrs;
	}
	else 
	{
		PBL_CORE_ASSERT_MSG(e == m_entity_component_lists.size(), "Entity out of range for add_entity!");

		// must be e == m_entity_component_lists.size(), so just add an extra element
		m_entity_component_lists.emplace_back(component_ptrs);
	}
}

void ComponentManager::remove_entity(Entity e)
{
	// has the entity been previously added and has it not been removed yet?
	if (e >= m_entity_component_lists.size() || m_entity_component_lists[e] == nullptr) return;

	// clean up entity component list
	m_entity_component_list_allocator.deallocate(m_entity_component_lists[e]);
	m_entity_component_lists[e] = nullptr;
}


// add, remove and get entity are templates implemented in ComponentManager.h


//
// ComponentChunkManager
//

ComponentManager::ComponentChunkManager::ComponentChunkManager(size_t component_size, size_t component_align, Allocator& chunk_allocator) :
																m_component_size(component_size),
																m_component_align(component_align),
																m_chunk_allocator(chunk_allocator)
{
	// at most the chunk could fit this many components,
	// ignoring align constraints and other reserved space in the chunk
	size_t max_components_per_chunk = COMPONENT_CHUNK_SIZE / m_component_size;

	m_flag_segments_per_chunk = (max_components_per_chunk % flag_segment_bitwidth) ?
											max_components_per_chunk / flag_segment_bitwidth + 1 :
											max_components_per_chunk / flag_segment_bitwidth;

	size_t remaining_chunk_space = COMPONENT_CHUNK_SIZE
									- sizeof(size_t) - alignof(size_t)
									- m_flag_segments_per_chunk * flag_segment_bitwidth
									- m_component_align;

	PBL_CORE_ASSERT_MSG(remaining_chunk_space > 0, "Not enough space in chunks for count and flags!");

	// the actual number of components we will put into each chunk,
	// wastefully handling align constraints (assuming it will be the worst in every chunk)
	m_components_per_chunk = remaining_chunk_space / m_component_size;

	PBL_CORE_ASSERT_MSG(m_components_per_chunk > 0, "Chunks cannot fit any components!");
}


ComponentManager::ComponentChunkManager::~ComponentChunkManager()
{
	// dealloc all chunks
	for (size_t i = 0; i < m_chunks.size(); ++i)
	{
		dealloc_chunk(i);
	}
}

/**
 * Allocate a new chunk and add it to the list of in-use chunks
 */
void ComponentManager::ComponentChunkManager::alloc_chunk()
{
	uintptr_t new_chunk = (uintptr_t)m_chunk_allocator.allocate(COMPONENT_CHUNK_SIZE, 0);

	m_chunks.push_back((uintptr_t)new_chunk); 

	// zero the component count field
	size_t* count = (size_t*)Util::manual_align(alignof(size_t), new_chunk);
	*count = 0;

	// zero all the flag segments
	FlagSegment* seg = (FlagSegment*)(count + 1);
	for (size_t i = 0; i < m_flag_segments_per_chunk; ++seg, ++i)
	{
		*seg = 0;
	}

	// seg now points just after the last flag segment,
	// so align seg to find the start of the components in the segment
	uintptr_t components = (uintptr_t)Util::manual_align(m_component_align, seg);

	m_chunks_components.push_back(components);
}

/**
 * Deallocate a currently in-use chunk, removing it from the list of in use chunks.
 * 
 * @param chunk_index the index into m_chunks of the chunk to dealloc
 */
void ComponentManager::ComponentChunkManager::dealloc_chunk(size_t chunk_index)
{
	PBL_CORE_ASSERT_MSG(*get_chunk_component_count(chunk_index) == 0, "Cannot deallocate a non-empty chunk!");

	void* chunk = (void*)m_chunks[chunk_index];

	m_chunk_allocator.deallocate(chunk);

	m_chunks.erase(m_chunks.begin() + chunk_index);
	m_chunks_components.erase(m_chunks_components.begin() + chunk_index);
}

/**
 * Allocates space for a component in the first avaliable chunk.
 * 
 * Allocates a new chunk if there is no free space in any chunk.
 * 
 * @return uintptr_t the location of the new component.
 */
uintptr_t ComponentManager::ComponentChunkManager::create_component()
{
	// TODO: find a better way to do this than iterating over all chunks from the start (make a better find_first_free_space method)
	// 			perhaps store the next free space, then advance it when used, would have to move it back if an earlier space opens
	size_t component_index = find_first_free_space();

	size_t chunk_index = component_index / m_components_per_chunk;
	size_t component_index_in_chunk = component_index % m_components_per_chunk;

	PBL_CORE_ASSERT_MSG(chunk_index <= m_chunks.size(), "Trying to create component {} in chunk {}, when we only have {} chunks!", component_index, chunk_index, m_chunks.size());

	if (chunk_index == m_chunks.size())
	{
		// need to add an extra chunk
		alloc_chunk();
	}

	set_chunk_flag(chunk_index, component_index_in_chunk);
	incr_chunk_component_count(chunk_index);

	uintptr_t component = m_chunks_components[chunk_index] + component_index_in_chunk * m_component_size;

	return component;
}

/**
 * Deallocates a component.
 * 
 * Deallocates a chunk if this was the last component in the chunk.
 * 
 * @param component the component to dealloc
 */
void ComponentManager::ComponentChunkManager::destroy_component(IComponent* component)
{
	uintptr_t component_location = (uintptr_t)component;

	// find index of chunk which holds component
	// basically see which address range the component is in
	size_t chunk_index = 0;
	for(; chunk_index < m_chunks_components.size(); ++chunk_index)
	{
		if (component_location >= m_chunks_components[chunk_index] && component_location < m_chunks_components[chunk_index] + COMPONENT_CHUNK_SIZE)
		{
			break;
		}
	}

	size_t component_index_in_chunk = (component_location - m_chunks_components[chunk_index]) / m_component_size;

	reset_chunk_flag(chunk_index, component_index_in_chunk);

	// if the chunk which held the component is now emty, free it
	if (decr_chunk_component_count(chunk_index) == 0)
	{
		dealloc_chunk(chunk_index);
	}
}

/**
 * Find the first index within all chunks which is not in use by an alive component.
 * 
 * The return value will be invalid if all chunks are full. It will be equal to m_chunks.size() * m_components_per_chunk in this case.
 * 
 * @return uintptr_t 
 */
uintptr_t ComponentManager::ComponentChunkManager::find_first_free_space() const
{
	size_t free_index = 0;
	bool found = false;

	for(size_t i = 0; i < m_chunks.size(); free_index += m_components_per_chunk, ++i)
	{
		if (*get_chunk_component_count(i) == m_components_per_chunk) continue;

		FlagSegment* flags = get_chunk_flags(i);
		for(size_t seg = 0; seg < m_flag_segments_per_chunk; ++seg)
		{
			if (flags[seg] == 0) 
			{
				free_index += seg * flag_segment_bitwidth;
				found = true;
				break;
			}

			FlagSegment s = ~flags[seg];
			if (s != 0)
			{
				s = (s & ~(s-1));
				int pos = 0;
				while (s >>= 1) ++pos;
				free_index += seg * flag_segment_bitwidth + pos;
				found = true;
				break;
			}
		}

		if (found) break;
	}

	return free_index;
}


// getters


/**
 * Get a pointer to the component count field of a chunk.
 */
inline size_t* ComponentManager::ComponentChunkManager::get_chunk_component_count(size_t chunk_index) const
{
	void* chunk = (void*)m_chunks[chunk_index];

	return (size_t*)(Util::manual_align(alignof(size_t), chunk));
}

/**
 * Get a pointer to the flags of a chunk.
 */
inline ComponentManager::ComponentChunkManager::FlagSegment* ComponentManager::ComponentChunkManager::get_chunk_flags(size_t chunk_index) const
{
	size_t* count = get_chunk_component_count(chunk_index);

	return (FlagSegment*)(count + 1);
}


// setters

/**
 * Increments the component count field of a chunk.
 * 
 * @return the new value of the size field, after increment.
 */
size_t ComponentManager::ComponentChunkManager::incr_chunk_component_count(size_t chunk_index)
{
	return ++(*get_chunk_component_count(chunk_index));
}

/**
 * Decrements the component count field of a chunk.
 * 
 * @return the new value of the size field, after decrement.
 */
size_t ComponentManager::ComponentChunkManager::decr_chunk_component_count(size_t chunk_index)
{
	return --(*get_chunk_component_count(chunk_index));
}

/**
 * Sets a flag for a chunk (set to 1).
 */
void ComponentManager::ComponentChunkManager::set_chunk_flag(size_t chunk_index, size_t flag_index)
{
	FlagSegment* segs = get_chunk_flags(chunk_index);

	segs[flag_index / flag_segment_bitwidth] |= BIT(flag_index % flag_segment_bitwidth);
}

/**
 * Resets a flag for a chunk (set to 0).
 */
void ComponentManager::ComponentChunkManager::reset_chunk_flag(size_t chunk_index, size_t flag_index)
{
	FlagSegment* segs = get_chunk_flags(chunk_index);

	segs[flag_index / flag_segment_bitwidth] &= ~(BIT(flag_index % flag_segment_bitwidth));
}


}