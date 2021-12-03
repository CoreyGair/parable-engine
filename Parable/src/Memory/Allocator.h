#pragma once

#include "pblpch.h"
#include "Core/Base.h"

namespace Parable
{


/**
 * Base interface for custom engine allocators.
 * 
 */
class Allocator
{
public:
    Allocator(size_t size, void* start) : m_size(size), m_start(start)
    {
        PBL_CORE_ASSERT_MSG(start != nullptr, "Cannot have an allocator start at null!")
    }
    virtual ~Allocator() {}

    // replace new & delete
    template<class T> T* allocate_new();
    template<class T> T* allocate_array(size_t length);
    template<class T> void deallocate_delete(T& object);
    template<class T> void deallocate_array(T* array);

    virtual void* allocate(size_t size, size_t alignment) = 0;
    virtual void  deallocate(void* p) = 0;

    size_t get_size() { return m_size; }
    void* get_start() { return m_start; }
    size_t get_used() { return m_used; }
    size_t get_allocations() { return m_allocations; }

protected:
    /**
     * The size in bytes of the allocation.
     */
    size_t m_size;
    /**
     * Address of the start of the allocation.
     */
    void* m_start;
    /**
     * The number of used bytes.
     */
    size_t m_used = 0;
    /**
     * The number of allocations made.
     */
    size_t m_allocations = 0;
};


}

#include "Allocator.tpp"