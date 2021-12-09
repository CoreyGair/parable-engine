#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Component.h"


namespace Parable::ECS
{


using SystemID = TypeID;

/**
 * Interface for systems.
 */
class ISystem
{
public:

	virtual ~ISystem() = 0;

	/**
	 * Performs the main work of the system, called each frame.
	 */
	virtual void on_update() = 0;

	/**
	 * Return the system id for this system (held in SystemTypeID).
	 */
	virtual SystemID get_system_id() const = 0;

	/**
	 * Should this system be called on update?
	 */
	bool enabled = true;

	int get_order() const { return m_order; }

protected:
	
	/**
	 * Update the order of this system.
	 *
	 * Should only be called in system constructor, as the order is only checked once by SystemManager at system creation.
	 */
	void set_order(int o) { m_order = o; }

private:
	/**
	 * Defines the order of execution for different systems.
	 * 
	 * Systems with a lower order are always executed before systems with higher orders.
	 * The execution order for systems with equal orders is undefined (and may change between updates).
	 */
	int m_order;
};

class SystemManager;

/**
 * Holds the type ID of the system class.
 *
 * @tparam S the derived (concrete) system class (CRTP).
 */
template<class S>
class System : public ISystem
{
	static SystemID system_id;

	friend SystemManager;

public:

	SystemID get_system_id() const override { return system_id; }
};

template<class S>
SystemID System<S>::system_id;

/**
 * Concept to check if type is a System.
 */
template<class T>
concept IsSystem = std::derived_from<T, System<T>>;

/**
 * Keeps track of the Component types the system wants to access.
 *
 * This is used in the engine to check which components each system depends on (e.g. for parralellism).
 * 
 * @tparam Components the types of component the system wants to access.
 */

template<IsComponent... Components>
class SystemComponentAccess
{
protected:
	/**
	 * Checks if this system requests permission to access a certain type of component.
	 * 
	 * Essentially checks if the requested type appears in the template args for the System<> class.
	 * 
	 * @tparam C the component to check if we have access to
	 * @return true if access to C was requested, false otherwise
	 */
	template<IsComponent C>
	constexpr bool has_permission() { return std::disjunction_v<std::is_same<C, Components>...>; }
};


}