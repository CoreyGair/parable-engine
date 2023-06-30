#pragma once

#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{


/**
 * Represents some range within a Buffer.
 * 
 * Note that a 0 size indicates a "null" slice.
 */
struct BufferSlice
{
    vk::DeviceSize start;
    vk::DeviceSize size;

    operator bool() { return size != 0; }
};


} 
