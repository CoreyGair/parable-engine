#pragma once

#include "Allocator.h"

namespace Parable
{


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