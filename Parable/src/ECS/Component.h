#pragma once

#include "pblpch.h"

#include "Core/Base.h"


namespace Parable::ECS
{


class ComponentManager;

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

template<class T>
class Component;

/**
 * Concept to check if type is a component type.
 */
template<class T>
concept IsComponent = std::derived_from<T, Component<T>>;

/**
 * Extension of IComponent to allow one m_component_type per actual component type.
 * 
 * @tparam T the type of the inheriting component class (CRTP)
 */
template<class T>
class Component : public IComponent
{
public:
	static ComponentTypeID get_static_component_type() { return m_component_type; }

private:
	static ComponentTypeID m_component_type;
	
	friend ComponentManager;
};

template<class T>
ComponentTypeID Component<T>::m_component_type;




}