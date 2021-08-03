#pragma once

#include "Allocator.h"

namespace Parable
{

/**
 * Simple allocator which allocates linearly.
 * 
 * This allocator cannot deallocate, instead it can only be completely cleared.
 * 
 */
class LinearAllocator : public Allocator
{
public:
    LinearAllocator(size_t size, void* start);
    ~LinearAllocator();

    void* allocate(size_t size, size_t alignment) override; 
    void deallocate(void* p) override; 
    void clear(); 

private:
    void* m_free_start;
};


}