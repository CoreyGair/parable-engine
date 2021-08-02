#include <gtest/gtest.h>

#include "test_with_malloc.h"

// engine includes
#include <Memory/LinearAllocator.h>
#include <Memory/PoolAllocator.h>


class TestLinearAllocator : public MallocWrapper<128>
{
public:
    TestLinearAllocator() : alloc(Parable::LinearAllocator(128, mem)) {}

protected:

    Parable::LinearAllocator alloc;
};

class TestPoolAllocator : public MallocWrapper<128>
{
public:
// use size_t as pool alloc cannot use a type smaller than void* (=size_t) (~8bytes)
    TestPoolAllocator() : alloc(Parable::PoolAllocator::create<size_t>(sizeof(size_t) * 10, mem)) {}
    
protected:

    Parable::PoolAllocator alloc;
};