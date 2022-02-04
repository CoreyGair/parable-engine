#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Entity.h"
#include "Component.h"

#include "Exception/ECSExceptions.h"


namespace Parable
{
	class Allocator;
	class PoolAllocator;
}

namespace Parable::ECS
{


class EntityComponentMap;

/**
 * Manages creation and storage of components for the ECS.
 */
class ComponentManager
{
public:
	ComponentManager(ComponentRegistry& registry, size_t total_chunks_allocation_size, size_t chunk_size, size_t entity_component_map_size, Allocator& allocator);
	~ComponentManager();

	void add_entity(Entity e);
	void remove_entity(Entity e);

	IComponent* add_component(Entity e, ComponentTypeID c);
	void remove_component(Entity e, ComponentTypeID c);
	IComponent* get_component(Entity e, ComponentTypeID c);
	bool has_component(Entity e, ComponentTypeID c);

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
		ComponentChunkManager(size_t chunk_size, size_t component_size, size_t component_align, Allocator& chunk_allocator);
		~ComponentChunkManager();

		// component mgmt
		uintptr_t create_component();
		void destroy_component(IComponent* component);

	private:
		/**
		 * The size in bytes of each chunk.
		 */
		size_t m_chunk_size;

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

	/**
	 * Checks if a component type is managed by this manager by comparing m_registry_id and Component<>::get_manager_id();
	 * 
	 * @tparam C the component type to check.
	 */
	template<IsComponent C>
	inline bool is_managed_by_this() { return m_registry_id == Component<C>::get_manager_id(); }

	/**
	 * The id of the registry used to create this ComponentManager.
	 * 
	 * Uniquely identifies a ComponentRegistry/ComponentManager pair, which manages a set of component types.
	 * All components managed by this manager will satisfy Component<T>::get_manager_id() == m_registry_id.
	 */
	TypeID m_registry_id;
	/**
	 * Count of registered components.
	 */
	const TypeID m_registered_components = 0;

	/**
	 * Maps entities to the components attached to them.
	 * 
	 * Should be the same map as the one referenced by the entity manager.
	 */
	UPtr<EntityComponentMap> m_entity_component_map;

	/**
	 * Vector of functions which call component constructors in-place.
	 * 
	 * Indexed by component type id.
	 */
	std::vector<void(*)(void*)> m_component_constructors;
	/**
	 * Vector of functions which call component destructors in-place.
	 *
	 * Indexed by component type id.
	 */
	std::vector<void(*)(void*)> m_component_destructors;
	/**
	 * Vector of functions which deregister the component types from this manager.
	 *
	 * Indexed by component type id.
	 */
	std::vector<void(*)(void)> m_component_deregisters;

	/**
	 * The main allocator used to store all component instances.
	 */
	Allocator& m_allocator;

	/**
	 * Allocates space for the component chunks.
	 */
	UPtr<PoolAllocator> m_component_chunk_allocator;
	/**
	 * List of chunk managers, indexed by ComponentTypeID.
	 * 
	 * Each manager manages chunks for the corresponding component.
	 */
	std::vector<ComponentChunkManager> m_chunk_managers;

};

/**
 * Registers components to be later managed by a ComponentManager.
 */
class ComponentRegistry
{
public:
	/**
	 * Construct a new registry with unique registry id.
	 * 
	 * Increments the static registry_count to give each registry a unique id.
	 */
	ComponentRegistry() : registry_id(++registry_count) {}

	/**
	 * Register a new component type to this registry.
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
		PBL_CORE_ASSERT_MSG(Component<T>::manager_id == 0, "Trying to register an already registered component type!");
		
		m_component_sizes.emplace_back(sizeof(T));
		m_component_aligns.emplace_back(alignof(T));

		m_component_constructors.emplace_back(&Component<T>::construct);
		m_component_destructors.emplace_back(&Component<T>::destruct);
		m_component_deregisters.emplace_back(&Component<T>::deregister);

		Component<T>::component_type = m_registered_components++;
		Component<T>::manager_id = registry_id;

		return Component<T>::component_type;
	}

	/**
	 * Uniquely identifies a ComponentRegistry/ComponentManager pair, which manages a set of component types.
	 */
	const TypeID registry_id;
	friend ComponentManager;

private:
	//privage getters used by ComponentManager

	ComponentTypeID get_num_registered() { return m_registered_components; }
	const std::vector<size_t>& get_sizes() { return m_component_sizes; }
	const std::vector<size_t>& get_aligns() { return m_component_aligns; }
	std::vector<void(*)(void*)>& get_ctors() { return m_component_constructors; }
	std::vector<void(*)(void*)>& get_dtors() { return m_component_destructors; }
	std::vector<void(*)(void)>& get_deregs() { return m_component_deregisters; }

	/**
	 * The number of component types registered to this registry.
	 */
	ComponentTypeID m_registered_components = 0;

	std::vector<size_t> m_component_sizes;
	std::vector<size_t> m_component_aligns;

	std::vector<void(*)(void*)> m_component_constructors;
	std::vector<void(*)(void*)> m_component_destructors;
	std::vector<void(*)(void)> m_component_deregisters;

	/**
	 * Global count of registries created, used to give each a unique id.
	 */
	static TypeID registry_count;

};


}