#include "PoolAllocator.h"


namespace Parable
{


/**
 * Construct a new Pool Allocator.
 * 
 * Splits the memory into chunks which are joined in a free chunk linked list.
 * Takes from the free chunks head when an allocation is requested
 * Inserts memory back into free list when it is deallocated.
 * 
 * @param object_size size of the chunks
 * @param object_alignment alignment of the chunks
 * @param alloc_size size of the allocated memory
 * @param alloc_start address of start of the allocated memory
 */
PoolAllocator::PoolAllocator(size_t object_size, size_t object_alignment, size_t alloc_size, void* alloc_start) :
                                Allocator(alloc_size, alloc_start), 
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
    // terminate the list with null
    *p = nullptr;
}

PoolAllocator::PoolAllocator(PoolAllocator&& other) : Allocator(other.get_size(), other.get_start())
{
    m_object_size = other.m_object_size;
    m_object_alignment = other.m_object_alignment;
    m_free_list = other.m_free_list;

    m_used = other.m_used;
    m_allocations = other.m_allocations;

    other.m_used = 0;
    other.m_allocations = 0;
}

PoolAllocator& PoolAllocator::operator=(PoolAllocator&& other)
{
    m_object_size = other.m_object_size;
    m_object_alignment = other.m_object_alignment;
    m_free_list = other.m_free_list;

    m_used = other.m_used;
    m_allocations = other.m_allocations;

    other.m_used = 0;
    other.m_allocations = 0;

    return *this;
}

PoolAllocator::~PoolAllocator()
{
    PBL_CORE_ASSERT_MSG(m_used == 0 && m_allocations == 0, "PoolAllocator memory leak! Used = {}, Allocs = {}", m_used, m_allocations);
}

/**
 * Allocate a memory chunk.
 * 
 * @param size the number of bytes to allocate
 * @param alignment the alignment required
 * @return void* address of allocated memory
 */
void* PoolAllocator::allocate(size_t size, size_t alignment)
{
    PBL_CORE_ASSERT_MSG(size == m_object_size, "PoolAllocator::allocate incorrect size {}, must be equal to the pool object size {}.", size, m_object_size)
    PBL_CORE_ASSERT_MSG(alignment == m_object_alignment, "PoolAllocator::allocate incorrect alignment {}, should be {}.", alignment, m_object_alignment)

    if (m_free_list == nullptr) return nullptr;

    void* return_ptr = m_free_list;

    // move the free list head on
    m_free_list = (void**)*m_free_list;

    m_used += m_object_size;
    ++m_allocations;

    return return_ptr;
}

/**
 * Deallocate a memory chunk.
 * 
 * Returns the memory to the free list to be reused.
 * 
 * @param p the memeory to deallocate
 */
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