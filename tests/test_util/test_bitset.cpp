#include <catch2/catch_test_macros.hpp>

#include <Util/DynamicBitset.h>
#include <Memory/LinearAllocator.h>

TEST_CASE("DynamicBitset set and get", "[util]")
{
    void* mem = malloc(10 * sizeof(size_t));
    Parable::LinearAllocator alloc (10 * sizeof(size_t),mem);

    Parable::Util::DynamicBitset bitset (10, alloc);

    REQUIRE(bitset.none() == true);

    SECTION("Set/get bit")
    {
        for(int i = 0; i < 10; ++i)
        {
            bitset.set(i);
            REQUIRE(bitset.at(i) == true);
            REQUIRE(bitset.any() == true);
            REQUIRE(bitset.count() == i+1);
        }

        REQUIRE(bitset.all() == true);

        SECTION("Reset bit")
        {
            for(int i = 0; i < 10; ++i)
            {
                bitset.reset(i);
                REQUIRE(bitset.at(i) == false);
            }
        }
    }

    SECTION("Flip")
    {
        bitset.flip_all();
        REQUIRE(bitset.all() == true);
    }
}

TEST_CASE("DynamicBitset operators", "[util]")
{
    void* mem = malloc(10 * sizeof(size_t));
    Parable::LinearAllocator alloc (10 * sizeof(size_t),mem);

    Parable::Util::DynamicBitset a (5, alloc);
    a.set(0); a.set(2); a.set(4);

    SECTION("Comparisons")
    {
        Parable::Util::DynamicBitset b (5, alloc);
        b.set(0); b.set(2); b.set(4);

        REQUIRE(a == b);

        b.reset_all();

        REQUIRE(a != b);
    }

    SECTION("Bitwise operations")
    {
        Parable::Util::DynamicBitset b (5, alloc);
        b.set(1); b.set(3);

        SECTION("AND")
        {
            b.set(0);
            a &= b;
            REQUIRE(a.count() == 1);
            REQUIRE(a.at(0) == true);
        }
        SECTION("OR")
        {
            a |= b;
            REQUIRE(a.all() == true);
        }
        SECTION("XOR")
        {
            b.set(0);
            a &= b;
            REQUIRE(a.count() == 4);
            REQUIRE(a.at(0) == false);
        }
    }
}