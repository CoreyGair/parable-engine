#include <gtest/gtest.h>

#include <Util/Pointer.h>

#include <memory>


TEST(TestManuaAlign, EquivalentToStl)
{
	// test a few times to sample a few different situations
	for(int i = 0; i < 20; ++i)
	{
		size_t size = 100;
		size_t align = 32;
		void* mem = malloc(size);

		void* test_mem = mem;
		EXPECT_EQ(Parable::Util::manual_align(align, test_mem), std::align(align, 8, test_mem, size));

		free(mem);
	}
}