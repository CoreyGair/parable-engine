#pragma once

#include "pblpch.h"
#include "Core/Base.h"

namespace Parable
{


// base interface for allocators
class Allocator
{
public:
    Allocator(size_t size, void* start) : m_size(size), m_start(start) {}
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
    size_t m_size;
    void* m_start;
    size_t m_used = 0;
    size_t m_allocations = 0;
};


}