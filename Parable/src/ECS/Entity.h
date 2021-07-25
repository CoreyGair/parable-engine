#pragma once

#include "Platform/PlatformDefines.h"

namespace Parable::ECS
{


using EntityID = TypeID;

class Entity
{
private:
    EntityID m_entity_id;
    bool m_active;

public:

    inline bool operator==(const Entity& rhs) const { return this->m_entity_id == rhs.m_entity_id; }
	inline bool operator!=(const Entity& rhs) const { return this->m_entity_id != rhs.m_entity_id; }
	inline bool operator==(const Entity* rhs) const { return this->m_entity_id == rhs->m_entity_id; }
	inline bool operator!=(const Entity* rhs) const { return this->m_entity_id != rhs->m_entity_id; }

    EntityID get_entity_id() { return m_entity_id; }

    void set_active(bool active) { m_active = active; }
    bool is_active() { return m_active; }
}


}