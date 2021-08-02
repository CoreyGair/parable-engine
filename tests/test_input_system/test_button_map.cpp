#include <gtest/gtest.h>

#include "test_button_map.h"

// includes from engine for test
#include "Input/ButtonMap.h"
#include "Events/InputEvent.h"


TEST_F(ButtonMapOneInput, PressAndReleasePolling)
{
    map.on_input_pressed(55);

    ASSERT_TRUE(map.is_down());
    ASSERT_TRUE(map.pressed_this_frame());

    map.on_update();
    EXPECT_FALSE(map.pressed_this_frame()) << "Update did not reset pressed state.";

    map.on_input_released(55);
    EXPECT_FALSE(map.is_down()) << "Release did not reset down state.";
}

TEST_F(ButtonMapOneInput, PressAndReleaseCallbacks)
{
    map.on_input_pressed(55);

    // TODO: implement callbacks and test 
    EXPECT_TRUE(false) << "implement callbacks and test";
}


TEST_F(ButtonMapMultipleInput, PressAndReleasePolling)
{
    std::vector<Parable::Input::InputCode> inputs {55,112,1};
    Parable::Input::ButtonMap map (std::string("multi input test"), inputs);

    map.on_input_pressed(55);
    map.on_input_pressed(112);
    map.on_input_pressed(1);

    ASSERT_TRUE(map.is_down());
    ASSERT_TRUE(map.pressed_this_frame());

    // only when all are released should down be reset
    map.on_input_released(55);
    EXPECT_TRUE(map.is_down());

    map.on_input_released(112);
    EXPECT_TRUE(map.is_down());

    map.on_input_released(1);
    EXPECT_FALSE(map.is_down());
}

TEST_F(ButtonMapMultipleInput, MultipleInputsCallbacks)
{
    map.on_input_pressed(55);
    map.on_input_pressed(112);
    map.on_input_pressed(1);

    // TODO: implement callbacks and test
    EXPECT_TRUE(false) << "implement callbacks and test";
}