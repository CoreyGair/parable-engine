#pragma once


namespace Parable::ECS
{


class IComponent
{
protected:
    bool m_enabled;

public:
    inline void set_enabled(bool enabled) { this->m_enabled = enabled; }
	inline bool is_enabled() const { return this->m_enabled; }
};


}