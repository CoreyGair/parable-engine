#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffer.h"
#include "BufferSlice.h"

namespace Parable::Vulkan
{


class BufferSuballocator
{
private:
    Buffer m_buffer;
    
    struct BufferFreeRange
    {
        vk::DeviceSize start;
        vk::DeviceSize size;

        vk::DeviceSize get_end() { return start + size; }

        bool operator<(const BufferFreeRange& rhs) { return size < rhs.size; }
    };

    /**
     * A list of the ranges within the buffer which are free in increasing size order.
     */
    std::vector<BufferFreeRange> m_free_ranges;

    /**
     * Insert a new BufferFreeRange into the correct space.
     */
    void insert_free_range(BufferFreeRange& free_range);

public:
    BufferSuballocator(Buffer buffer)
        : m_buffer(buffer), m_free_ranges({{0,buffer.get_size()}})
    {}

    Buffer& get_buffer() { return m_buffer; }

    void destroy()
    {
        m_buffer.destroy();
    }

    /**
     * Allocate a slice from the buffer.
     * 
     * @param allocation_size The size of the slice to be allocated.
     * @return BufferSlice The allocated slice. The size will be 0 if allocation failed.
     */
    BufferSlice allocate(vk::DeviceSize allocation_size);

    /**
     * Release a slice back to the buffer.
     * 
     * Releasing a slice twice or releasing a slice not allocated from this buffer results in undefined behaviour.
     * 
     * @param slice The slice to release.
     */
    void free(BufferSlice slice);
};




}
