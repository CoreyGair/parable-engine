#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Component.h"


namespace Parable::ECS
{


class SystemManager;
class ECS;

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

// define PVD
inline ISystem::~ISystem() {}

class SystemManager;

/**
 * Holds the type ID of the system class.
 *
 * @tparam S the derived (concrete) system class (CRTP).
 */
template<class S>
class System : public ISystem
{
	/**
	 * An identifier for the system, unique to this system type (not object).
	 */
	static SystemID system_id;

	/**
	 * The ecs this system type is managed by.
	 */
	static ECS& m_ecs;

	friend SystemManager;

protected:
	/**
	 * Get the ecs this system is managed by.
	 * 
	 * Should be used in system implementations to reference the ecs and access entity/component/system managers.
	 */
	inline static ECS& get_ecs() { return m_ecs; }

public:
	static SystemID get_static_system_id() { return system_id; }
};

template<class S>
SystemID System<S>::system_id;

template<class S>
ECS& System<S>::m_ecs;

/**
 * Concept to check if type is a System.
 */
template<class T>
concept IsSystem = std::derived_from<T, System<T>>;

// TODO: actually implement this "functionality request" behaviour
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