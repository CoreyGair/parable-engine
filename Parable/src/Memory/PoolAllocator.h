#pragma once

#include "Allocator.h"

namespace Parable
{


/**
 * Allocates fixed-size chunks which are re-used after deallocation.
 * 
 * Basically splits the memory into an array of chunks, which are re-used to keep the data dense.
 * 
 */
class PoolAllocator : public Allocator
{
public:
    PoolAllocator(size_t object_size, size_t object_alignment, size_t alloc_size, void* alloc_start);
    PoolAllocator(PoolAllocator&& other);
    ~PoolAllocator();

    /**
     * Convenience factory method to avoid passing sizeof()'s and alignof()'s
     * 
     * @tparam T type of object which this pool will store
     * @param alloc_size size of the allocated memory
     * @param alloc_start address of the start of the allocation
     */
    template<class T>
    static PoolAllocator create(size_t alloc_size, void* alloc_start)
    {
        return PoolAllocator(sizeof(T), alignof(T), alloc_size, alloc_start);
    }

    void* allocate(size_t size, size_t alignment) override;
    void  deallocate(void* p) override;

private:
    /**
     * Pool chunk size (the size of the objects this pool is storing).
     */
    size_t m_object_size;
    /**
     * Alignment of the objects this pool is storing.
     */
    size_t m_object_alignment;
    /**
     * Head of the linked list of free chunks.
     */
    void** m_free_list;

};


}