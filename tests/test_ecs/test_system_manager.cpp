
#include <gtest/gtest.h>

#include "test_system_manager.h"


TEST_F(SystemManagerSingleton, SystemOrderRespected)
{
    manager.on_update();

    EXPECT_EQ(A::updates, 1);
    EXPECT_EQ(B::updates, 1);
    EXPECT_EQ(C::updates, 1);

    EXPECT_EQ(side_effects, "ABC");
}

TEST_F(SystemManagerSingleton, SystemEnableRespected)
{
    manager.set_enabled<B>(false);

    manager.on_update();

    EXPECT_EQ(A::updates, 1);
    EXPECT_EQ(B::updates, 0);
    EXPECT_EQ(C::updates, 1);

    EXPECT_EQ(side_effects, "AC");
}