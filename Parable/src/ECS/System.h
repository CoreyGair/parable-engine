#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Component.h"


namespace Parable::ECS
{


/**
 * Abstract 
 */
class ISystem
{
public:
	/**
	 * Performs the main work of the system, called each frame.
	 */
	virtual void on_update() = 0;

	/**
	 * Defines the order of execution for different systems.
	 * 
	 * Systems with a lower order are always executed before systems with higher orders.
	 * The execution order for systems with equal orders is undefined (and may change between updates).
	 */
	virtual int get_order() { return 0; }

	/**
	 * Should this system be called on update?
	 */
	bool m_enabled;
};


/**
 * Keeps track of the Component types the system wants to access.
 * 
 * @tparam Components the types of component the system wants to access.
 */
template<IsComponent... Components>
class System : public ISystem
{
private:
	/**
	 * Checks if this system requests permission to access a certain type of component.
	 * 
	 * Essentially checks if the requested type appears in the template args for the System<> class.
	 * 
	 * @tparam C the component to check if we have access to
	 * @return true if access to C was requested, false otherwise
	 */
	template<class C>
	constexpr bool has_permission() { return std::disjunction_v<std::is_same<C, Components>...>; }
};


/**
 * Concept to check if type is a system.
 */
template<class T>
concept IsSystem = std::derived_from<T, ISystem>;

}