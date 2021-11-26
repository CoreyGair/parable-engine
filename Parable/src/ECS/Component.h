#pragma once

#include "Core/Base.h"


namespace Parable::ECS
{


using ComponentTypeID = TypeID;

class IComponent
{
private:
	static ComponentTypeID m_component_type;

public:
	ComponentTypeID get_component_type() { return m_component_type; }
};

template<class T>
class Component : public IComponent
{

};


}