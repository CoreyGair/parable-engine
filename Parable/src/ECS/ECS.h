#pragma once

#include "pblpch.h"
#include "Core/Base.h"

#include "ComponentManager.h"

namespace Parable
{
class Allocator;
}


namespace Parable::ECS
{

class EntityManager;
class SystemManager;
class EntityComponentMap;
class ComponentRegistry;


/**
 * Controls the lifetimes of a set of objects which make up a single ECS, as well as providing an interface to those objects.
 */
class ECS
{
public:

	~ECS();

	void on_update();

	Entity create_entity();
	void destroy_entity(Entity e);

	/**
	 * Creates and adds a component type to an alive entity.
	 * 	 
	 * @param e the entity to add the component to
	 * @tparam C the component type to create
	 * @return C* the newly created component, or the already attached component if the entity already had one attached.
	 * @throws IncorrectManagerException if the component type is not managed by the ComponentManager of this ECS.
	 */
	template<IsComponent C>
	C* add_component(Entity e) { return (C*)m_component_manager->add_component(e, Component<C>::get_component_type()); }

	/**
	 * Removes and destroys a component type from an alive entity.
	 * 
	 * Does nothing if no component of this type was attached to the entity.
	 * 
	 * @param e the entity to remove the component from
	 * @tparam C the component type to remove
	 * @throws IncorrectManagerException if the component type is not managed by the ComponentManager of this ECS.
	 */
	template<IsComponent C>
	void remove_component(Entity e) { m_component_manager->remove_component(e, Component<C>::get_component_type()); }

	/**
	 * Gets a pointer to a component attached to an alive entity.
	 * 
	 * Null if this component type is not attached to the entity.
	 * 
	 * @param e the entity to find the component on
	 * @tparam C the type of component to find
	 * @throws IncorrectManagerException if the component type is not managed by the ComponentManager of this ECS.
	 */
	template<IsComponent C>
	C* get_component(Entity e) { return (C*)m_component_manager->get_component(e, Component<C>::get_component_type()); }

	/**
	 * Checks if a component is attached to an alive entity.
	 * 
	 * @tparam C the component type to check for.
	 * @param e the entity to check for the component on.
	 * @throws IncorrectManagerException if the component type is not managed by the ComponentManager of this ECS.
	 */
	template<IsComponent C>
	bool has_component(Entity e) { return m_component_manager->has_component(e, Component<C>::get_component_type()); }

	class ECSBuilder
	{
	public:
		ECSBuilder();

		UPtr<ECS> create();

		void set_entity_component_map_size(size_t s) { m_entity_component_map_size = s; }
		void set_component_chunk_size(size_t s) { m_component_chunk_size = s; }
		void set_component_chunks_total_size(size_t s) { m_component_chunks_total_size = s; }

		ComponentRegistry* get_registry() { return m_component_registry.get(); }

	private:
		size_t m_entity_component_map_size = 0;
		size_t m_component_chunk_size = 0;
		size_t m_component_chunks_total_size = 0;

		UPtr<ComponentRegistry> m_component_registry;

		bool created = false;
	};
	
private:
	/**
	 * Construct a new ECS object.
	 * 
	 * ECS objects can only be constructed by an ECSBuilder.
	 */
	ECS(UPtr<EntityManager>, UPtr<ComponentManager>, UPtr<SystemManager>, UPtr<Allocator>);

	UPtr<EntityManager> m_entity_manager;
	UPtr<ComponentManager> m_component_manager;
	UPtr<SystemManager> m_system_manager;

	UPtr<Allocator> m_allocator;
};

}