#pragma once

#include "Core/Base.h"

#include <cstdlib>

namespace Parable::Util
{


/**
 * RAII wrapper for a simple malloc buffer.
 */
class Buffer
{
public:
    Buffer(size_t size)
    {
        m_data = std::malloc(size);
        PBL_CORE_ASSERT(m_data != nullptr);
    }

    Buffer(const Buffer& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;

    Buffer(Buffer&& other)
    {
        m_data = other.m_data;
        other.m_data = nullptr;
    }
    Buffer& operator=(Buffer&& other)
    {
        m_data = other.m_data;
        other.m_data = nullptr;
    }

    ~Buffer()
    {
        if (m_data)
        {
            std::free(m_data);
        }
    }

    void* get() const { return m_data; }

    /**
     * Retrieve a pointer to some byte offset from the start of the buffer.
     */
    void* get_offset(size_t offset) const { return static_cast<void*>(static_cast<char*>(m_data) + offset); }

private:
    void* m_data;
}


}
