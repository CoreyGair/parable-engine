#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

// includes from engine for test
#include "Input/ButtonMap.h"
#include "Events/InputEvent.h"


TEST_CASE("ButtonMap functions with one input", "[input]")
{
    std::vector<Parable::Input::InputCode> inputs {55};
    Parable::Input::ButtonMap map (std::string("test"), inputs);

    REQUIRE(map.get_name() == "test");

    SECTION("Button pressed sets down and pressed states")
    {
        map.on_input_pressed(55);

        REQUIRE(map.is_down() == true);
        REQUIRE(map.pressed_this_frame() == true);

        SECTION("Update resets pressed state")
        {
            map.on_update();

            REQUIRE(map.pressed_this_frame() == false);
        }
        SECTION("Release resets down state")
        {
            map.on_input_released(55);

            REQUIRE(map.is_down() == false);
        }
    }
    SECTION("Button pressed calls pressed callback")
    {
        // TODO: implement callbacks and test
        REQUIRE(false);

        SECTION("Button released calls released callback")
        {
            REQUIRE(false);
        }
    }
}

TEST_CASE("ButtonMap functions with multiple input", "[input]")
{
    std::vector<Parable::Input::InputCode> inputs {55,112,1};
    Parable::Input::ButtonMap map (std::string("multi input test"), inputs);

    SECTION("Buttons pressed sets down and pressed states")
    {
        map.on_input_pressed(55);
        map.on_input_pressed(112);
        map.on_input_pressed(1);

        REQUIRE(map.is_down() == true);
        REQUIRE(map.pressed_this_frame() == true);
    }
    SECTION("Multiple pressed doesnt notify pressed callback multiple times")
    {
        // TODO: implement callbacks and test
        REQUIRE(false);

        SECTION("Only calls released callback when all released")
        {
            REQUIRE(false);
        }
    }
}