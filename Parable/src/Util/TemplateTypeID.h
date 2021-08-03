#pragma once

#include "Platform/PlatformDefines.h"

namespace Parable::Util
{

/**
 * Generates compile-time evaluated ID's for derived classes.
 */
template<class T>
class TemplateTypeID
{
	static TypeID s_count;
	
public:
	
	/**
	 * Gives a compile-time evaluated ID for derived classes.
	 * 
	 * For the interface class T, a base class Base and a derived class U, we have:
	 * 
	 * U -isa-> Base<U> -isa-> T
	 * 
	 * When this class is called with templates T & U, gives a unique ID for U
	 */
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