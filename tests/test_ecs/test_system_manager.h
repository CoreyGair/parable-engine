
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
        A::updates = 0;
        B::updates = 0;
        C::updates = 0;

        side_effects = "";
    }

    // void TearDown() override {}

    Parable::ECS::SystemManager manager;

    // test systems

    static std::string side_effects;

    class A : public Parable::ECS::System<A>
    {
    public:
        A() { set_order(-1); }

        ~A() override = default;

        void on_update() override { ++updates; side_effects += 'A'; }

        static int updates;
    };
    class B: public Parable::ECS::System<B>
    {
    public:
        B() { set_order(0); }

        ~B() = default;

        void on_update() override { ++updates; side_effects += 'B'; }

        static int updates;
    };
    class C : public Parable::ECS::System<C>
    {
    public:
        C() { set_order(1); }

        ~C() = default;

        void on_update() override { ++updates; side_effects += 'C'; }

        static int updates;
    };
};