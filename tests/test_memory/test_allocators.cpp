#include <gtest/gtest.h>

#include "test_allocators.h"

// engine includes
#include <Memory/LinearAllocator.h>
#include <Memory/PoolAllocator.h>


// NOTE: we dont test deallocation here as LinearAllocator doesnt dealloc, only clear
TEST_F(TestLinearAllocator, SingleAllocation)
{
    int* x = alloc.allocate_new<int>();
    ASSERT_NE(x, nullptr);

    // test an access to proc any hidden segfaults
    *x = 1;

    alloc.clear();
}
TEST_F(TestLinearAllocator, ArrayAllocation)
{
    int* arr = alloc.allocate_array<int>(4);
    ASSERT_NE(arr, nullptr);

    arr[0] = 1;
    arr[1] = 1;
    arr[2] = 1;
    arr[3] = 1;

    alloc.clear();
}


TEST_F(TestPoolAllocator, SingleAllocation)
{
    size_t* x = alloc.allocate_new<size_t>();
    ASSERT_NE(x, nullptr);

    // test an access to proc any hidden segfaults
    *x = 1;

    alloc.deallocate_delete(*x);
    EXPECT_EQ(alloc.get_used(), 0) << "Used memory is not 0.";
    EXPECT_EQ(alloc.get_allocations(), 0) << "Not all allocations have been deallocated.";
}
TEST_F(TestPoolAllocator, ArrayAllocation)
{
    size_t* arr = alloc.allocate_array<size_t>(4);
    ASSERT_NE(arr, nullptr);

    arr[0] = 1;
    arr[1] = 1;
    arr[2] = 1;
    arr[3] = 1;

    alloc.deallocate_array(arr);
    EXPECT_EQ(alloc.get_used(), 0) << "Used memory is not 0.";
    EXPECT_EQ(alloc.get_allocations(), 0) << "Not all allocations have been deallocated.";
}