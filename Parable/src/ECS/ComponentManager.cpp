#include "ComponentManager.h"

#include "EntityComponentMap.h"

#include "Exception/MemoryExceptions.h"

#include "Util/Pointer.h"

#include "Memory/Allocator.h"
#include "Memory/PoolAllocator.h"


namespace Parable::ECS
{


TypeID ComponentRegistry::registry_count = 0;

/**
 * Construct a new ComponentManager from an existing ComponentRegistry.
 * 
 * @param registry holds the component types this manager will manage.
 * @param chunks_allocation_size the number of bytes to allocate for storing component chunks.
 * @param chunk_size the number of bytes to allocate per component chunk.
 * @param allocator the allocator from which to request memory.
 */
ComponentManager::ComponentManager(ComponentRegistry& registry, size_t total_chunks_allocation_size, size_t chunk_size, size_t entity_component_map_size, Allocator& allocator) :
															m_allocator(allocator),
															m_registered_components(registry.get_num_registered()),
															m_entity_component_map(
																std::make_unique<EntityComponentMap>(
																	registry.get_num_registered(),
																	entity_component_map_size,
																	allocator
																)
															),
															m_component_chunk_allocator(
																std::make_unique<PoolAllocator>(
																	chunk_size,
																	0,
																	total_chunks_allocation_size,
																	allocator.allocate(total_chunks_allocation_size, 0)
																)
															),
															m_component_constructors(std::move(registry.get_ctors())),
															m_component_destructors(std::move(registry.get_dtors())),
															m_component_deregisters(std::move(registry.get_deregs()))
{
	std::cout<<"A";
	// unpack component type data from registry
	const std::vector<size_t>& component_sizes = registry.get_sizes();
	const std::vector<size_t>& component_aligns = registry.get_aligns();

	// create chunk managers
	for(ComponentTypeID i = 0; i < m_registered_components; ++i)
	{
		m_chunk_managers.emplace_back(chunk_size, component_sizes[i], component_aligns[i], *m_component_chunk_allocator);
	}

	// save overallocated memory
	m_chunk_managers.shrink_to_fit();
}

ComponentManager::~ComponentManager()
{
	// deallocate the component chunks
	m_allocator.deallocate(m_component_chunk_allocator->get_start());

	// deregister all component types so they may be registered to another manager if needed
	for(auto f : m_component_deregisters) f();
}

/**
 * Notify the ComponentManager of a newly created entity.
 */
void ComponentManager::add_entity(Entity e)
{
	m_entity_component_map->add_entity(e);
}

/**
 * Notify the ComponentManager that an entity was destroyed.
 */
void ComponentManager::remove_entity(Entity e)
{
	// remove all entitys components first
	for (ComponentTypeID i = 0; i < m_registered_components; ++i)
	{
		remove_component(e,i);
	}

	m_entity_component_map->remove_entity(e);
}

/**
 * Allocates space for a new component, calls its default constructor and attaches it to an entity.
 */
IComponent* ComponentManager::add_component(Entity e, ComponentTypeID c)
{
	if (has_component(e, c)) return (*m_entity_component_map)[e][c];

	// request a new component from the relevant ChunkManager
	IComponent* component = (IComponent*)m_chunk_managers[c].create_component();

	// call default constructor on the new component location
	m_component_constructors[c](component);

	// add component to the entity
	(*m_entity_component_map)[e][c] = component;

	return component;
}

/**
 * Destructs & deallocates a component and removes it from an entity.
 */
void ComponentManager::remove_component(Entity e, ComponentTypeID c)
{
	if (!has_component(e, c)) return;

	IComponent* component = (*m_entity_component_map)[e][c];

	// call dtor on the new component location
	m_component_destructors[c](component);

	// dealloc the component
	m_chunk_managers[c].destroy_component(component);

	// remove from entity
	(*m_entity_component_map)[e][c] = nullptr;
}

/**
 * Find a component attached to an entity.
 */
IComponent* ComponentManager::get_component(Entity e, ComponentTypeID c)
{
	if (!has_component(e, c)) return nullptr;

	return (*m_entity_component_map)[e][c];
}

bool ComponentManager::has_component(Entity e, ComponentTypeID c)
{
	if (e >= (*m_entity_component_map).get_map().size()) throw NullEntityException((std::string("Entity ") + std::to_string(e) + std::string(" is not currently in use!")).c_str());

    // TODO: think the first part of this should be in the above throw
    //      if (*m_entity_component_map)[e] is null then e is not an entity
	return (*m_entity_component_map)[e] != nullptr && (*m_entity_component_map)[e][c] != nullptr;
}


//
// ComponentChunkManager
//

/**
 * Construct a new ComponentChunkManager.
 * 
 * @param component_size the size of the components stored in the chunk.
 * @param component_align the alignment of the components stored in the chunk.
 * @param chunk_allocator the allocator from which to request new chunks.
 */
ComponentManager::ComponentChunkManager::ComponentChunkManager(size_t chunk_size, size_t component_size, size_t component_align, Allocator& chunk_allocator) :
																m_chunk_size(chunk_size),
																m_component_size(component_size),
																m_component_align(component_align),
																m_chunk_allocator(chunk_allocator)
{
	// at most the chunk could fit this many components,
	// ignoring align constraints and other reserved space in the chunk
	size_t max_components_per_chunk = m_chunk_size / m_component_size;

	m_flag_segments_per_chunk = (max_components_per_chunk % flag_segment_bitwidth) ?
											max_components_per_chunk / flag_segment_bitwidth + 1 :
											max_components_per_chunk / flag_segment_bitwidth;

	size_t remaining_chunk_space = m_chunk_size
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
	uintptr_t new_chunk = (uintptr_t)m_chunk_allocator.allocate(m_chunk_size, 0);

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
		if (component_location >= m_chunks_components[chunk_index] && component_location < m_chunks_components[chunk_index] + m_chunk_size)
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