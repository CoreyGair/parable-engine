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


}