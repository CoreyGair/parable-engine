#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

// includes from engine for test
#include "Input/Axis.h"
#include "Events/InputEvent.h"

// possibly should fuzz the nums for these, but catch doesnt do it and i dont want to implement it

TEST_CASE("MouseXAxis functions correctly", "[input]")
{
    Parable::Input::MouseXAxis axis;

    SECTION("Recieving a positive mouse x delta increases value correctly")
    {
        Parable::Event* positive_x_event = new Parable::MouseMovedEvent(2.5,0.0);

        axis.on_event(positive_x_event);
        
        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == 2.5);
    }

    SECTION("Recieving a negative mouse x delta decreases value correctly")
    {
        Parable::Event* negative_x_event = new Parable::MouseMovedEvent(-1.8,0.0);

        axis.on_event(negative_x_event);

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == -1.8);
    }
}

TEST_CASE("MouseYAxis functions correctly", "[input]")
{
    Parable::Input::MouseYAxis axis;

    SECTION("Recieving a positive mouse y delta increases value correctly")
    {
        Parable::Event* positive_y_event = new Parable::MouseMovedEvent(0.0,3.55);

        axis.on_event(positive_y_event);

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == 3.55);
    }

    SECTION("Recieving a negative mouse y delta decreases value correctly")
    {
        Parable::Event* negative_y_event = new Parable::MouseMovedEvent(0.0,-0.87);

        axis.on_event(negative_y_event);

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == -0.87);
    }
}

TEST_CASE("MouseScrollAxis functions correctly", "[input]")
{
    Parable::Input::MouseScrollAxis axis;

    SECTION("Recieving a positive mouse scroll increases value correctly")
    {
        Parable::Event* positive_scroll_event = new Parable::MouseScrolledEvent(2.0);

        axis.on_event(positive_scroll_event);

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == 2.0);
    }

    SECTION("Recieving a negative mouse scroll decreases value correctly")
    {
        Parable::Event* negative_scroll_event = new Parable::MouseScrolledEvent(-3.5);

        axis.on_event(negative_scroll_event);

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == -3.5);
    }
}

// utility for below tests
static Parable::Event::EventUPtr create_press_event(Parable::Input::InputCode btn)
{
    if (btn <= 8)
    {
        return std::make_unique<Parable::MouseBtnPressedEvent>(btn);
    }
    else
    {
        return std::make_unique<Parable::KeyPressedEvent>(btn);
    }
}
static Parable::Event::EventUPtr create_release_event(Parable::Input::InputCode btn)
{
    if (btn <= 8)
    {
        return std::make_unique<Parable::MouseBtnReleasedEvent>(btn);
    }
    else
    {
        return std::make_unique<Parable::KeyReleasedEvent>(btn);
    }
}

TEST_CASE("ButtonAxis functions correctly", "[input]")
{
    Parable::Input::ButtonAxis axis (std::vector<Parable::Input::InputCode> {1,55}, std::vector<Parable::Input::InputCode> {2,97});

    // keyboard tests
    SECTION("Recieving a positive key press sets value correctly")
    {
        Parable::Input::InputCode button = GENERATE(55,1);
        Parable::Event::EventUPtr positive_pressed_event = create_press_event(button);

        axis.on_event(positive_pressed_event.get());

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == 1);

        SECTION("Recieving positive button released resets value")
        {
            Parable::Event::EventUPtr positive_released_event = create_release_event(button);

            axis.on_event(positive_released_event.get());

            REQUIRE(axis.dirty());
            REQUIRE(axis.get_value() == 0);
        }
    }

    SECTION("Recieving a negative button press sets value correctly")
    {
        Parable::Input::InputCode button = GENERATE(97,2);
        Parable::Event::EventUPtr negative_pressed_event = create_press_event(button);

        axis.on_event(negative_pressed_event.get());

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == -1);

        SECTION("Recieving negative button released resets value")
        {
            Parable::Event::EventUPtr negative_released_event = create_release_event(button);

            axis.on_event(negative_released_event.get());

            REQUIRE(axis.dirty());
            REQUIRE(axis.get_value() == 0);
        }
    }

    SECTION("Pressing multiple positive buttons gives correct value")
    {
        Parable::Event::EventUPtr positive_pressed_event_1 = create_press_event(55);
        Parable::Event::EventUPtr positive_pressed_event_2 = create_press_event(1);

        axis.on_event(positive_pressed_event_1.get());
        axis.on_event(positive_pressed_event_2.get());

        REQUIRE(axis.dirty());
        REQUIRE(axis.get_value() == 1);
    }
}