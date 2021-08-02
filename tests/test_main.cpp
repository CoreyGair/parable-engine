#include <gtest/gtest.h>

#include <Core/Log.h>


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    Parable::Log::init();

    return RUN_ALL_TESTS();
}