#pragma once

#include "Core/Base.h"

#include <functional>

#include "System.h"

namespace Parable::ECS
{


class SystemManager
{
public:

	void on_update();

	// system handling

	template<IsSystem S>
	void add_system()
	{
		// create the system object
		UPtr<S> system = std::make_unique<S>();

		// insert ref in correct order slot
		m_systems_by_order.insert(std::upper_bound(m_systems_by_order.begin(), m_systems_by_order.end(),
								*system,
								[](const ISystem& a, const ISystem& b) { return a.get_order() < b.get_order(); }
								),
							*system
							);

		// update the system type id
		System<S>::system_id = m_systems_by_id.size();
		m_systems_by_id.emplace_back(std::move(system));
	}

	template<IsSystem S>
	void set_enabled(bool enabled)
	{
		const SystemID id = System<S>::system_id;

		m_systems_by_id.at(id)->enabled = enabled;
	}
	

private:

	/**
	 * The systems to be executed, ordered by the systems order member.
	 *
	 * Contains the same elements as m_systems_by_id, but they are instead ordered by ISystem.get_order().
	 * Used for iterating over systems to call them in the correct order.
	 */
	std::vector<std::reference_wrapper<ISystem>> m_systems_by_order;

	/**
	 * The systems to be executed, ordered (and indexed) by the systems id instead.
	 *
	 * Contains the same elements as m_systems_by_order, but they are instead ordered by SystemIDContainer's.
	 * Used for getting systems by id, saves us from searching in m_systems.
	 */
	std::vector<UPtr<ISystem>> m_systems_by_id;
};


}