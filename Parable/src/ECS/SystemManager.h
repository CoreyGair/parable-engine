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
		UPtr<S> system = UPtr<S>::make_unique();

		// update the system type id
		SystemIDContainer<S>::m_system_id = m_systems_by_id.size();
		m_systems_by_id.emplace_back(*system);

		// move into systems vector
		m_systems.insert(std::upper_bound(m_systems.begin(), m_systems.end(),
								system,
								[](const UPtr<ISystem>& a, const UPtr<ISystem>& b) { return a->get_order() < b->get_order(); }
								),
							std::move(system)
							);
	}

	template<IsSystem S>
	void set_enabled(bool enabled)
	{
		const SystemID id = SystemIDContainer<S>::m_system_id;

		m_systems_by_id.at(id).get().enabled = enabled;
	}
	

private:

	/**
	 * The actual systems to be executed, ordered by the systems order member.
	 */
	std::vector<UPtr<ISystem>> m_systems;

	/**
	 * m_systems, ordered (and indexed) by the systems id instead.
	 *
	 * Contains the same elements as m_systems, but they are instead ordered by SystemIDContainer's.
	 * Used for getting systems by id, saves us from searching in m_systems.
	 */
	std::vector<std::reference_wrapper<ISystem>> m_systems_by_id;
};


}