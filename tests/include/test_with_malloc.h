#pragma once

#include <gtest/gtest.h>

template <size_t MALLOC_SIZE>
class MallocWrapper : public ::testing::Test
{
public:
    MallocWrapper() : mem(malloc(MALLOC_SIZE)) {}
    ~MallocWrapper()
    {
        free(mem);
    }

    void* mem;
};