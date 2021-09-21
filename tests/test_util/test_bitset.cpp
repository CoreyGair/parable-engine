#include <gtest/gtest.h>

#include "test_bitset.h"

#include <Util/DynamicBitset.h>
#include <Memory/LinearAllocator.h>


// DYNAMIC BITSET TESTS

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

    ASSERT_NE(a, b) << "Inequality test failed";

    EXPECT_TRUE(b.is_subset_of(a)) << "subset check failed";

    b.set(0); b.set(2); b.set(4);
    ASSERT_EQ(a, b) << "Equality test failed";
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


// STATIC BITSET
#include <Util/StaticBitset.tpp>

TEST_F(TestStaticBitset, SetAndGet)
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

TEST_F(TestStaticBitset, FindFirst)
{
    bitset.set(2); bitset.set(5);
    EXPECT_EQ(bitset.find_first_set(), 2);

    std::vector<size_t> indexes_set ({2,5});
    EXPECT_EQ(bitset.get_set_bits(), indexes_set);

    std::vector<size_t> indexes_unset ({0,1,3,4,6,7,8,9});
    EXPECT_EQ(bitset.get_unset_bits(), indexes_unset);

    bitset.set(0); bitset.set(1);
    EXPECT_EQ(bitset.find_first_unset(), 3);

    bitset.reset_all();
    EXPECT_EQ(bitset.find_first_unset(), 0);
    bitset.set(0);
    EXPECT_EQ(bitset.find_first_unset(), 1);
    bitset.set(1);
    EXPECT_EQ(bitset.find_first_unset(), 2);
}

TEST_F(TestTwoStaticBitset, Comparisons)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0);

    ASSERT_NE(a, b) << "Inequality test failed";

    EXPECT_TRUE(b.is_subset_of(a)) << "subset check failed";

    b.set(0); b.set(2); b.set(4);
    ASSERT_EQ(a, b) << "Equality test failed";
}
TEST_F(TestTwoStaticBitset, BitwiseAND)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0); b.set(1); b.set(3);

    a &= b;
    EXPECT_EQ(a.count(), 1);
    EXPECT_TRUE(a.at(0));
}
TEST_F(TestTwoStaticBitset, BitwiseOR)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0); b.set(1); b.set(3);

    a |= b;
    EXPECT_TRUE(a.all());
}
TEST_F(TestTwoStaticBitset, BitwiseXOR)
{
    a.set(0); a.set(2); a.set(4);
    b.set(0); b.set(1); b.set(3);

    a ^= b;
    EXPECT_EQ(a.count(), 4);
    EXPECT_FALSE(a.at(0));
}