#pragma once

#include "Platform/PlatformDefines.h"

namespace Parable::Util
{

//// NOTE: In order to use this, must put the following in the header of the InterfaceClass (with correct class names):
/*

namespace Parable::Util
{
TypeID TemplateTypeID<InterfaceClass>::s_count = 0;
template class TemplateTypeID<InterfaceClass>;
}

*/

/**
 * Generates compile-time evaluated ID's for derived classes.
 */
template<class T>
class TemplateTypeID
{
	static TypeID s_count;
	
public:
	
	/**
	 * Gives a compile-time evaluated ID for derived classes (uses CRTP).
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

	static constexpr TypeID get_count()
	{
		return s_count;
	}
};


}