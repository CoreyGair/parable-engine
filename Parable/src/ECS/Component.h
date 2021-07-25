#pragma once

#include "IComponent.h"

#include "Util/TemplateTypeID.h"
#include "Platform/PlatformDefines.h"

namespace Parable::ECS
{


template<class T>
class Component : public IComponent
{
public:
	static const TypeID s_component_type_id;

	Component() {}
	virtual ~Component() {}		

	inline TypeID GetStaticComponentTypeID() const
	{
		return STATIC_COMPONENT_TYPE_ID;
	}	
};

//  define the static type id
//  incremented at compile time with each new T 
template<class T>
const TypeID Component<T>::s_component_type_id = ::Util::TemplateTypeID<IComponent>::Get<T>();


}