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
class Exception : public std::exception
{
public:
    Exception(const char* msg) : m_msg(msg) {}
    Exception(const std::string& msg) : m_msg(msg.c_str()) {}

    const char* what() const noexcept override { return m_msg; }

private:
    const char* m_msg;
};


}