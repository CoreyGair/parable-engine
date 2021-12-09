#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Entity.h"
#include "Component.h"

#include "Memory/Allocator.h"
#include "Memory/PoolAllocator.h"

namespace Parable::ECS
{


// TODO: kind of want to refactor this whole thing into pure data oriented / functional / without objects
// remvoe some complexity overhead of the mess of objects here
// like remove ChunkManager obj and just manage an array of std::vector<chunk> indexed by component type
/**
 * Manages creation and storage for components of the ECS.
 * 
 * This includes registering user-defined components, dynamic memory allocation and attaching components to Entities.
 */
class ComponentManager
{
public:
	ComponentManager(size_t entity_component_lists_size, size_t component_chunk_size, Allocator& allocator);
	~ComponentManager();

	void init();
	bool m_is_initialised = false;

	/**
	 * Register a new component type with the ECS.
	 * 
	 * This sets Component<T>::m_component_type.
	 * 
	 * @tparam T the component class which is being registered.
	 * 
	 * @return The TypeID for the component.
	 */
	template<class T>
	TypeID register_component()
	{
		PBL_CORE_ASSERT_MSG(!m_is_initialised, "Trying to register component after ComponentManager::Init()!");

		Component<T>::m_component_type = m_registered_components++;

		m_component_sizes.emplace_back(sizeof(T));
		m_component_aligns.emplace_back(alignof(T));
		return Component<T>::m_component_type;
	}

	void add_entity(Entity e);
	void remove_entity(Entity e);

	template<IsComponent C>
	C* add_component(Entity e);

	template<IsComponent C>
	void remove_component(Entity e);

	template<IsComponent C>
	C* get_component(Entity e);

private:
	/**
	 * Manages chunks of memory for storing one type of component.
	 * 
	 * Each chunk managed by an instance of ComponentChunkManager will contain space for m_components_per_chunk components.
	 * 
	 * They will also have a prefixed size_t field indicating the number of components currently in the chunk,
	 * as well as an array of FlagSegment (length m_flag_segments_per_chunk) indicating which slots contain live components.
	 * 
	 * All pointer management is done with uintptr_t, to avoid type confusion. Instead, type casting is handled by the owning ComponentManager.
	 */
	class ComponentChunkManager
	{
	public:
		ComponentChunkManager(size_t component_size, size_t component_align, Allocator& chunk_allocator);
		~ComponentChunkManager();

		// component mgmt
		uintptr_t create_component();
		void destroy_component(IComponent* component);

	private:
		// FlagSegment used as bit flags to indicate which slots in the chunk contain active components
		// bit[i] are set if component[i] is used
		using FlagSegment = unsigned char;

		// mem mgmt
		void alloc_chunk();
		void dealloc_chunk(size_t chunk_index);

		// utility
		uintptr_t find_first_free_space() const;

		// getters
		inline size_t* get_chunk_component_count(size_t chunk_index) const;
		inline FlagSegment* get_chunk_flags(size_t chunk_index) const;

		// setters
		size_t incr_chunk_component_count(size_t chunk_index);
		size_t decr_chunk_component_count(size_t chunk_index);
		void set_chunk_flag(size_t chunk_index, size_t flag_index);
		void reset_chunk_flag(size_t chunk_index, size_t flag_index);

		// vars
		Allocator& m_chunk_allocator;

		size_t m_component_size;
		size_t m_component_align;

		// the implementation assumes alignof(FlagSegment) == 1
		static_assert(alignof(FlagSegment) == 1, "FlagSegment is required to have alignment 1.");
		static constexpr size_t flag_segment_bitwidth = sizeof(FlagSegment) * 8;

		size_t m_flag_segments_per_chunk;

		size_t m_components_per_chunk;

		// TODO: want to avoid vector/dynamic alloc but this will do for now
		// maybe just make chunks a linked list (have a void*/uintptr_t field before the components, like the flags/count field)
		/**
		 * Pointers to the chunks currently managed by this object.
		 * 
		 * Alloc & dealloc of these is up to this ComponentChunkManager, using m_chunk_allocator.
		 */
		std::vector<uintptr_t> m_chunks;

		/**
		 * Points to the start of the components within chunks.
		 * 
		 * Indexed the same as m_chunks.
		 */
		std::vector<uintptr_t> m_chunks_components;
	};

private:
	IComponent* create_component(ComponentTypeID);

private:
	// TODO: Static ecs class which manages constants like this
	/**
	 * Number of bytes to allocate to entity component lists.
	 */
	static size_t ENTITY_COMPONENT_LISTS_SIZE;
	/**
	 * Number of bytes to allocate per component chunk.
	 */
	static size_t COMPONENT_CHUNK_SIZE;
	/**
	 * The main allocator used to store all component instances.
	 */
	Allocator& m_allocator;

	/**
	 * Count of registered components.
	 * 
	 * Will not change after Init() called.
	 */
	TypeID m_registered_components = 0;

	// TODO: maybe make this a new 'ExpandableAllocator'
	// atm, pool allocs fixed size arrays of IComponent*
	// could make expandable by allowing it to malloc() a chunk of N (like 100) Icomponent* arrays,
	// as this currently limits the number of entitites we can handle to the ammount prealloced at the start
	/**
	 * Allocates space for the entity component lists.
	 */
	PoolAllocator m_entity_component_list_allocator;
	/**
	 * Stores lists of pointers to components for each currently alive entity.
	 * 
	 * m_entity_component_lists[e] is an array of Component* attached to the Entity e.
	 * 
	 * m_entity_component_lists[e][c] is the Component* for the ComponentTypeID c attached to the Entity e.
	 */
	std::vector<IComponent**> m_entity_component_lists;

	/**
	 * Vector of sizes of registered component types, indexed by ComponentTypeID.
	 */
	std::vector<size_t> m_component_sizes;
	/**
	 * Vector of alignments of registered component types, indexed by ComponentTypeID.
	 */
	std::vector<size_t> m_component_aligns;

	/**
	 * Allocates space for the component chunks.
	 * 
	 * Holds pools of COMPONENT_CHUNK_SIZE chunks to be assigned to ComponentChunk objects.
	 */
	PoolAllocator m_component_chunk_allocator;
	/**
	 * List of chunk managers, indexed by ComponentTypeID.
	 * 
	 * Each manager manages chunks for the corresponding component.
	 */
	std::vector<ComponentChunkManager> m_chunk_managers;

};

/**
 * Create a new component and attach it to an entity.
 * 
 * Note that the created component will be initialised using the component constructor with no arguments.
 * 
 * @param e the entity to add the component to
 * @tparam C the component type to add
 * @return IComponent* the newly created component
 */
template<IsComponent C>
C* ComponentManager::add_component(Entity e)
{
	ComponentTypeID component_type = Component<C>::get_static_component_type();

	// request a new component from the relevant ChunkManager
	C* component = new ((C*)m_chunk_managers[component_type].create_component()) C;

	// add component to the entity
	m_entity_component_lists[e][component_type] = (IComponent*)component;

	return component;
}


/**
 * Remove a component from an entity, and dealloc the component.
 * 
 * @param e the entity to remove the component from
 * @tparam C the component type to remove
 */
template<IsComponent C>
void ComponentManager::remove_component(Entity e)
{
	ComponentTypeID component_type = Component<C>::get_static_component_type();

	IComponent* component = m_entity_component_lists[e][component_type];

	// must manually destruct components
	((C*)component)->~C();

	// dealloc the component
	m_chunk_managers[component_type].destroy_component(component);

	// remove from entity
	m_entity_component_lists[e][component_type] = nullptr;
}

/**
 * Get a pointer to a component attached to an entity. Null if the component is not attached to the entity.
 * 
 * @param e the entity to find the component on
 * @tparam C the component type to find
 */
template<IsComponent C>
C* ComponentManager::get_component(Entity e)
{
	ComponentTypeID component_type = Component<C>::get_static_component_type();

	return (C*)m_entity_component_lists[e][component_type];
}




}