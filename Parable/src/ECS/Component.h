#pragma once

#include "Core/Base.h"


namespace Parable::ECS
{


class ComponentManager;

using ComponentTypeID = TypeID;

/**
 * Interface used by ComponentManager to dynamically handle component types.
 * 
 * Any IComponent* returned by the ComponentManager is gauranteed to be castable to a pointer to the derived component class (whatever inherits from Component) with m_component_type.
 * 
 */
class IComponent
{
public:

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
	static ComponentTypeID get_static_component_type() { return m_component_type; }

private:
	static ComponentTypeID m_component_type;

	friend ComponentManager;
};

template<class T>
ComponentTypeID Component<T>::m_component_type;


}