#include <gtest/gtest.h>

#include "test_with_malloc.h"

#include <Util/DynamicBitset.h>
#include <Memory/LinearAllocator.h>

class TestDynamicBitset : public MallocWrapper<256>
{
public:
    TestDynamicBitset() : alloc(Parable::LinearAllocator(15 * sizeof(size_t), mem)),
                            bitset(Parable::Util::DynamicBitset(10, alloc)) {}

    ~TestDynamicBitset()
    {
        alloc.clear();
    }

    Parable::LinearAllocator alloc;
    Parable::Util::DynamicBitset bitset;
};

class TestTwoDynamicBitset : public MallocWrapper<256>
{
public:
    TestTwoDynamicBitset() : alloc(Parable::LinearAllocator(15 * sizeof(size_t), mem)),
                                a(Parable::Util::DynamicBitset(5, alloc)),
                                b(Parable::Util::DynamicBitset(5, alloc)) {}

    ~TestTwoDynamicBitset()
    {
        alloc.clear();
    }
    Parable::LinearAllocator alloc;
    Parable::Util::DynamicBitset a;
    Parable::Util::DynamicBitset b;
};