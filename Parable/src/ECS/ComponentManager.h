#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Entity.h"
#include "Component.h"

#include "Memory/Allocator.h"
#include "Memory/PoolAllocator.h"

namespace Parable::ECS
{


// TODO: Static ecs class which manages constants like this
const size_t m_ENTITY_COMPONENT_LISTS_ALLOC_SIZE = 4000; // bytes to alloc to entity component lists
const size_t m_COMPONENT_CHUNK_SIZE = 8000; // bytes to alloc per component chunk

class ComponentManager
{
public:
	ComponentManager(Allocator& allocator);
	~ComponentManager();

	void Init();
	bool m_is_initialised = false;

	/**
	 * Register a new component type with the ECS.
	 * 
	 * @tparam T the component class which is being registered.
	 * 
	 * @return The TypeID for the component.
	 */
	template<class T>
	TypeID register_component()
	{
		PBL_CORE_ASSERT_MSG(!m_is_initialised, "Trying to register component after ComponentManager::Init()!");
		m_component_sizes.emplace_back(sizeof(T));
		m_component_aligns.emplace_back(alignof(T));
		return m_registered_components++;
	}

	void add_entity(Entity e);
	void remove_entity(Entity e);

	void add_component(Entity e, ComponentTypeID c);
	void remove_component(Entity e, ComponentTypeID c);

private:
	/**
	 * Stores a fixed amount of a single component type.
	 * 
	 * Can be linked together into a linked list.
	 */
	struct ComponentChunk
	{
		ComponentChunk(size_t chunk_size, size_t component_size, size_t component_align);

		IComponent* get(size_t i) { return (IComponent*)(i < m_component_count ? ((uintptr_t)m_components + i*m_component_size) : (uintptr_t)nullptr);}
		size_t get_component_count() { return m_component_count; }
		IComponent* get_components_array() { return m_components; }

		ComponentChunk* m_next;
		const size_t m_component_size;

	private:
		IComponent* m_components;
		size_t m_component_count;
	};

private:
	IComponent* create_component(ComponentTypeID);

private:
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

	/**
	 * Allocates space for the entity component lists.
	 */
	PoolAllocator m_entity_component_list_allocator;
	/**
	 * Stores lists of pointers to components for each currently alive entity.
	 * 
	 * m_entity_component_lists[e] is a list of Component* attached to the Entity e.
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
	 */
	PoolAllocator m_component_chunk_allocator;
	/**
	 * Vector of ComponentChunk lists.
	 * 
	 * Each list (indexed by ComponentTypeID) holds the head of a list of ComponentChunks containing Components of type ComponentTypeID.
	 */
	std::vector<ComponentChunk*> m_component_chunk_lists;
};





}