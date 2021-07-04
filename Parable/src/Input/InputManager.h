#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"
#include "Input/InputContext.h"

#include <glm/glm.hpp>

namespace Parable::Input
{


class InputManager
{
public:
    static InputManager* s_instance; // singleton
    static InputManager& get_instance() { return *s_instance; }

    InputManager();
    ~InputManager() { s_instance = nullptr; }

    void on_update();
    void on_event(Event* e);

private:
    std::vector<InputContext> m_contexts;
};


}