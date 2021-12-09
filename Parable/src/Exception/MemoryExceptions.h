#pragma once

#include "Exception.h"

namespace Parable
{


/**
 * Thrown when a memory allocation fails.
 */
class AllocationFailedException : public Exception
{
public:
    using Exception::Exception;
};

/**
 * Thrown when a data structure using dynamic allocation runs out of memory.
 */
class OutOfMemoryException : public Exception
{
public:
    using Exception::Exception;
};


}