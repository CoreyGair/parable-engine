#pragma once

#include "pblpch.h"

#include <exception>

namespace Parable
{


class Exception : std::exception
{
public:
    Exception(const char* msg) : std::exception(msg) {}
};


}