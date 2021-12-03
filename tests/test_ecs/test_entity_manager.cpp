#include <gtest/gtest.h>

#include "test_entity_manager.h"


TEST_F(EntityManagerSingleton, Create)
{
	EXPECT_EQ(manager.create(), 0);
	EXPECT_EQ(manager.create(), 1);
	EXPECT_EQ(manager.create(), 2);
}

TEST_F(EntityManagerSingleton, Reuse)
{
	EXPECT_EQ(manager.create(), 0);
	EXPECT_EQ(manager.create(), 1);
	EXPECT_EQ(manager.create(), 2);
	EXPECT_EQ(manager.create(), 3);
	EXPECT_EQ(manager.create(), 4);

	manager.destroy(1);
	manager.destroy(3);

	EXPECT_EQ(manager.create(), 1);
	EXPECT_EQ(manager.create(), 3);
	EXPECT_EQ(manager.create(), 5);
}