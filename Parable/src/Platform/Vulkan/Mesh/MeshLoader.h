#pragma once

#include <vulkan/vulkan.hpp>

#include "../Wrapper/PhysicalDevice.h"
#include "../Wrapper/Device.h"
#include "../Wrapper/CommandPool.h"

#include "Mesh.h"

namespace Parable
{
class MeshLoadInfo;
}

namespace Parable::Vulkan
{


class BufferSuballocator;

class MeshLoader
{
private:
    PhysicalDevice m_physical_device;
    Device m_device;
    
    vk::Queue m_transfer_queue;

    CommandPool m_command_pool;

    vk::Fence m_transfer_finished_fence;

public:
    MeshLoader(Device device, PhysicalDevice physical_device, vk::Queue transfer_queue, uint32_t transfer_family);

    Mesh load(const MeshLoadInfo& load_info, BufferSuballocator& vertex_buffer_allocator, BufferSuballocator& index_buffer_allocator);
};


}
