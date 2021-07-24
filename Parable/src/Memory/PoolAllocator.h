#pragma once

#include "Allocator.h"

namespace Parable
{


class PoolAllocator : public Allocator
{
public:
    PoolAllocator(size_t object_size, size_t object_alignment, size_t alloc_size, void* alloc_start);
    ~PoolAllocator();

    // convenience factory method to avoid passing sizeof()'s and alignof()'s
    template<class T>
    static PoolAllocator create(size_t alloc_size, void* alloc_start)
    {
        return PoolAllocator(sizeof(T), alignof(T), alloc_size, alloc_start);
    }

    void* allocate(size_t size, size_t alignment) override;
    void  deallocate(void* p) override;

private:
    size_t m_object_size;
    size_t m_object_alignment;
    void** m_free_list;

};


}