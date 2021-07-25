#include "PoolAllocator.h"


namespace Parable
{


PoolAllocator::PoolAllocator(size_t object_size, size_t object_alignment, size_t alloc_size, void* alloc_start) :                                 Allocator(alloc_size, alloc_start), 
                                m_object_size(object_size),
                                m_object_alignment(object_alignment)
{
    PBL_CORE_ASSERT_MSG(object_size >= sizeof(void*), "PoolAllocator cannot use objects smaller than void*.")

    // align the alloc_start pointer to the specified allignment
    // also reducecs alloc_size by adjustment
    std::align(object_alignment, object_size, alloc_start, alloc_size);

    size_t num_objects = alloc_size / object_size;

    m_free_list = (void**)alloc_start;
    void** p = m_free_list;

    // make the free space into a linked list of free objects
    for(size_t i = 0; i < num_objects-1; ++i)
    {
        // make *p point to the next space
        *p = (void*)((uintptr_t)p + object_size);
        // move p along to the next space
        p = (void**)*p;
    }
    // teninate the list with null
    *p = nullptr;
}

PoolAllocator::~PoolAllocator()
{
    PBL_CORE_ASSERT_MSG(m_used == 0 && m_allocations == 0, "PoolAllocator memory leak!")
}

void* PoolAllocator::allocate(size_t size, size_t alignment)
{
    PBL_CORE_ASSERT_MSG(size == m_object_size, "PoolAllocator::allocate incorrect size {}, should be {}.", size, m_object_size)
    PBL_CORE_ASSERT_MSG(alignment == m_object_alignment, "PoolAllocator::allocate incorrect alignment {}, should be {}.", alignment, m_object_alignment)

    if (m_free_list == nullptr) return nullptr;

    void* return_ptr = m_free_list;

    // move the free list head on
    m_free_list = (void**)*m_free_list;

    m_used += m_object_size;
    ++m_allocations;

    return return_ptr;
}

void PoolAllocator::deallocate(void* p) 
{
    // put this ptr at the head of the free list:
    // cast to void** and set to point at old head
    *((void**)p) = m_free_list; 
    // set head to be p
    m_free_list = (void**)p; 

    m_used -= m_object_size; 
    m_allocations--;
} 


}