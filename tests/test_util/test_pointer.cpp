#include <gtest/gtest.h>

#include <Util/Pointer.h>

#include <memory>


TEST(TestManuaAlign, EquivalentToStl)
{
	size_t size = 100;
	size_t align = 32;
	void* mem = malloc(size);

	void* test_mem = mem;
	EXPECT_EQ(Parable::Util::manual_align(align, test_mem), std::align(align, 8, test_mem, size));

	test_mem = mem;
	EXPECT_EQ(Parable::Util::manual_align(align, test_mem), std::align(align, 8, test_mem, size));
}