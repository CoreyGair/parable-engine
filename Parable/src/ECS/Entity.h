#pragma once

#include 'Core/Base.h'



namespace Parable::ECS
{


/**
 * Entity, represented as unique 64-bit ID.
 *
 * 64-bit num can represent all the seconds since the big bang, so hopefully we won't run out of ID's in an engine runtime.
 */
using Entity = uint64_t;


}