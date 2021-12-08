
#include <gtest/gtest.h>

#include <ECS/SystemManager.h>

class SystemManagerSingleton : public ::testing::Test
{
public:
    SystemManagerSingleton()
    {
        manager.add_system<A>();
        manager.add_system<B>();
        manager.add_system<C>();
    }

protected:
    void SetUp() override
    {
        A::value = 0;
        B::value = 0;
        C::value = 0;

        side_effects = "";
    }

    // void TearDown() override {}

    Parable::ECS::SystemManager manager;

    // test systems

    static std::string side_effects = "";

    class A : public Parable::ECS::System<A>
    {
    public:
        A() : m_order(-1) {}

        void on_update() override { updates += 1; side_effects += 'A'; }

        static int updates = 0;
    }
    class B: public Parable::ECS::System<B>
    {
    public:
        B() : m_order(0) {}

        void on_update() override { updates += 1; side_effects += 'B'; }

        static int updates = 0;
    }
    class C : public Parable::ECS::System<C>
    {
    public:
        C() : m_order(1) {}

        void on_update() override { updates += 1; side_effects += 'C'; }

        static int updates = 0;
    }
};