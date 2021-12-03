#include <gtest/gtest.h>

#include <ECS/EntityManager.h>

class EntityManagerSingleton : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }

    // void TearDown() override {}

    Parable::ECS::EntityManager manager;
};