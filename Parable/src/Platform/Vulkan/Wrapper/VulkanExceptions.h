#pragma once

#include "Exception/Exception.h"

#include <format>

#include <vulkan/vulkan.h>


namespace Parable
{


class NoGraphicsDeviceException : public Exception
{
public:
    NoGraphicsDeviceException() : Exception("No suitable graphics devices.") {}
};

class VulkanResourceException : public Exception
{
public:
    using Exception::Exception;
};

class VulkanDeviceAllocationFailed : public Exception
{
public:
    VulkanDeviceAllocationFailed(VkResult reason) : Exception(std::format("Failed to allocate device memory, error code {}.", static_cast<int>(reason))) {}
};

class VulkanFailedCreateException : public Exception
{
public:
    VulkanFailedCreateException(const char* type, VkResult reason) : Exception(std::format("Failed to create {}, error code {}.", type, static_cast<int>(reason))) {}
};


}