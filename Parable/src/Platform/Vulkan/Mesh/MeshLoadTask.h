#pragma once

#include "pblpch.h"

#include "../Loader/LoadTask.h"
#include "../Wrapper/Buffer.h"

namespace Parable
{
class MeshLoadInfo;
}

namespace vk
{
class CommabdBuffer;
}

namespace Parable::Vulkan
{


class MeshStateBlock;

class Device;
class PhysicalDevice;
class BufferSuballocator;

/**
 * A MeshLoadTask represents the task of loading a mesh (vertex + index data) into GPU buffers.
 */
class MeshLoadTask : public LoadTask
{
private:
    const MeshLoadInfo& m_load_info;

    MeshStateBlock& m_mesh_state;

    BufferSuballocator& m_vertex_target_suballocator;
    BufferSuballocator& m_index_target_suballocator;

    Buffer m_vertex_staging_buffer;
    Buffer m_index_staging_buffer;

public:
    MeshLoadTask(
        const MeshLoadInfo& load_info,
        MeshStateBlock& mesh_state,
        BufferSuballocator& vertex_target_suballocator,
        BufferSuballocator& index_target_suballocator
    );

    /**
     * Record buffer copy commands.
     */
    void record_commands(Device& device, PhysicalDevice& physical_device, vk::CommandBuffer& command_buffer) override;
    /**
     * Release staging buffer resources.
     */
    void on_load_complete() override;
};


}

