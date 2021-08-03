#include "LinearAllocator.h"


namespace Parable
{


LinearAllocator::LinearAllocator(size_t size, void* start) :
                                                    Allocator(size, start),
                                                    m_free_start(start)
                                                    { PBL_CORE_ASSERT_MSG(size > 0, "LinearAllocator given size of 0!") }

LinearAllocator::~LinearAllocator()
{
    PBL_CORE_ASSERT_MSG(m_used == 0 && m_allocations == 0, "LinearAllocator memory leak!")
}

/**
 * Allocate memory.
 * 
 * @param size the number of bytes to allocate
 * @param alignment the alignment required
 * @return void* address of allocated memory
 */
void* LinearAllocator::allocate(size_t size, size_t alignment)
{
    PBL_CORE_ASSERT_MSG(size != 0, "Trying to allocate with size 0!")

    // get an aligned addr within the free space (std::align gauranteed to return the first avaliable one)
    void* aligned_free = m_free_start;
    size_t free_size = m_size - m_used;
    size_t free_size_after_alignment = free_size;
    std::align(alignment, size, aligned_free, free_size_after_alignment);
    
    if (aligned_free != nullptr)
    {
        // add the alignment padding + alloc size to used, move the free ptr to the aligned addr + alloc size
        m_used += (free_size - free_size_after_alignment) + size;
        m_free_start = (void*)((uintptr_t)aligned_free + size);
        ++m_allocations;
    }

    return aligned_free;
}

/**
 * This allocator does not deallocate; use clear() instead.
 */
void LinearAllocator::deallocate(void* p)
{
    PBL_CORE_WARN("Cannot dealloc from LinearAllocator, use clear() instead.");
}

/**
 * Clear all allocations.
 * 
 */
void LinearAllocator::clear()
{
    m_used = 0;
    m_allocations = 0;
    m_free_start = m_start;
}


}