#pragma once

#include "pblpch.h"

#include "Core/Base.h"


namespace Parable::ECS
{


class ComponentRegistry;

using ComponentTypeID = TypeID;

// TODO: ISystem might be obsolete: actually has no use apart from acting as a pointer to some component.
//			Instead, could have ComponentManager use void* or uintptr_t internally, this type is not needed.
/**
 * Interface used by ComponentManager to dynamically handle component types.
 * 
 * Any IComponent* returned by the ComponentManager is gauranteed to be castable to a pointer to the derived component class (whatever inherits from Component) with m_component_type.
 * 
 */
class IComponent
{

};

/**
 * Extension of IComponent to allow one m_component_type per actual component type.
 * 
 * @tparam T the type of the inheriting component class (CRTP)
 */
template<class T>
class Component : public IComponent
{
public:
	static ComponentTypeID get_component_type() { return component_type; }
	static TypeID get_manager_id() { return manager_id; }

	/**
	 * Default construct in place.
	 */
	static void construct(void* location) { new ((T*)location) T; }

	/**
	 * Destruct in place.
	 */
	static void destruct(void* location) { ((T*)location)->~T(); }

	/**
	 * Deregister the component type.
	 */
	static void deregister() { manager_id = 0; }

private:
	/**
	 * Unique identifier for the concrete component type T.
	 */
	static ComponentTypeID component_type;

	/**
	 * Which object manages this component type?
	 * 
	 * 0 is reserved as 'unregistered'.
	 */
	static TypeID manager_id;
	
	friend ComponentRegistry;
};

template<class T>
ComponentTypeID Component<T>::component_type;

template<class T>
TypeID Component<T>::manager_id = 0;

/**
 * Concept to check if type is a component type.
 */
template<class T>
concept IsComponent = std::derived_from<T, Component<T>>;


}