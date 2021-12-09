
#include <gtest/gtest.h>

#include "test_system_manager.h"

std::string SystemManagerSingleton::side_effects = "";

int SystemManagerSingleton::A::updates = 0;
int SystemManagerSingleton::B::updates = 0;
int SystemManagerSingleton::C::updates = 0;

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