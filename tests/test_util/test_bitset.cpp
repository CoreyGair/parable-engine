#include <gtest/gtest.h>

#include "test_bitset.h"

#include <Util/DynamicBitset.h>
#include <Memory/LinearAllocator.h>


TEST_F(TestDynamicBitset, SetAndGet)
{
    ASSERT_TRUE(bitset.none()) << "Bitset not initialised with 0's";

    // set all
    for(int i = 0; i < 10; ++i)
    {
        bitset.set(i);
        ASSERT_TRUE(bitset.at(i)) << "Failed to set bit " << i;
        EXPECT_TRUE(bitset.any()) << "any() check failed";
        EXPECT_EQ(bitset.count(), i+1) << "count is incorrect";
    }

    ASSERT_TRUE(bitset.all()) << "all() check failed";

    // reset all
    for(int i = 0; i < 10; ++i)
    {
        bitset.reset(i);
        EXPECT_FALSE(bitset.at(i)) << "failed to reset bit " << i;
    }

    ASSERT_TRUE(bitset.none()) << "none() check failed";

    bitset.flip_all();
    EXPECT_TRUE(bitset.all()) << "flip failed";
}

TEST_F(TestTwoDynamicBitset, Comparisons)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0);

    ASSERT_NE(a, b);

    b.set(0); b.set(2); b.set(4);
    ASSERT_EQ(a, b);
}
TEST_F(TestTwoDynamicBitset, BitwiseAND)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0); b.set(1); b.set(3);

    a &= b;
    EXPECT_EQ(a.count(), 1);
    EXPECT_TRUE(a.at(0));
}
TEST_F(TestTwoDynamicBitset, BitwiseOR)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0); b.set(1); b.set(3);

    a |= b;
    EXPECT_TRUE(a.all());
}
TEST_F(TestTwoDynamicBitset, BitwiseXOR)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0); b.set(1); b.set(3);

    a ^= b;
    EXPECT_EQ(a.count(), 4);
    EXPECT_FALSE(a.at(0));
}