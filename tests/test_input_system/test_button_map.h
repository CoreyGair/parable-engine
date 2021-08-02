#include <gtest/gtest.h>

// includes from engine for test
#include "Input/ButtonMap.h"
#include "Events/InputEvent.h"

class ButtonMapOneInput : public ::testing::Test
{
public:
    ButtonMapOneInput() : map(Parable::Input::ButtonMap(std::string("ButtonMapOneInput"), std::vector<Parable::Input::InputCode>({55}))) {}

protected:
    void SetUp() override
    {

    }

    // void TearDown() override {}

    Parable::Input::ButtonMap map;
};

class ButtonMapMultipleInput : public ::testing::Test
{
public:
    ButtonMapMultipleInput() : map(Parable::Input::ButtonMap(std::string("ButtonMapOneInput"), std::vector<Parable::Input::InputCode>({55,112,1}))) {}
protected:
    void SetUp() override
    {

    }

    // void TearDown() override {}

    Parable::Input::ButtonMap map;
};
