#pragma once

#include "Exception.h"

namespace Parable
{


/**
 * Thrown when an argument violates a bound on accessing a containers.
 */
class OutOfRangeException : public Exception
{
public:
    using Exception::Exception;
};

/**
 * Thrown when an object has state which is incompatible with an action which was attempted.
 */
class IncompatibleObjectException : public Exception
{
public:
    using Exception::Exception;
};


}