#pragma once

#include "pblpch.h"

#include <exception>

namespace Parable
{

/**
 * Base exception class for Parable exceptions.
 * 
 * Essntially just a wrapper for std::exception.
 */
class Exception : std::exception
{
public:
    Exception(const char* msg) : std::exception(msg) {}
};


}