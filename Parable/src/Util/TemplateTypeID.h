#pragma once

#include "Platform/PlatformDefines.h"

namespace Parable::Util
{

// use with inheritance -> Derived = U : Base<Derived> : Interface = T

// gives a compile-time evaluated ID for derived classes in the above pattern

template<class T>
class TemplateTypeID
{
	static TypeID s_count;
	
public:
	
	template<class U>
	static const TypeID get()
	{
		static const TypeID STATIC_TYPE_ID { s_count++ };
		return STATIC_TYPE_ID;
	}

	static const TypeID get()
	{
		return s_count;
	}
};


}