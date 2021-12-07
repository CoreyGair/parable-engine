#pragma once

#include "System.h"

namespace Parable::ECS
{


class SystemManager
{
public:
	SystemManager();
	~SystemManager();

	void register_system(ISystem&& system) { m_systems.push_back(std::move(system)); }
	

private:
	/**
	 * The actual systems to be executed.
	 */
	std::vector<ISystem> m_systems;

}


}