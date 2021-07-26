#pragma once

#include "Exception.h"

namespace Parable
{


class AllocationFailedException : public Exception
{
public:
    using Exception::Exception;
};


}