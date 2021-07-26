#pragma once

#include "Exception.h"

namespace Parable
{


class OutOfRangeException : public Exception
{
public:
    using Exception::Exception;
};

class IncompatibleObjectException : public Exception
{
public:
    using Exception::Exception;
};


}