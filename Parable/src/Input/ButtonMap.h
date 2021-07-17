#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"

namespace Parable::Input
{


using ButtonCallback = std::function<void(void)>;

class ButtonMap{
public:
    ButtonMap(std::string& name, std::vector<InputCode>& inputs) : m_name(std::move(name)), m_inputs(std::move(inputs)) {}

    const std::string& get_name() { return m_name; }

    bool is_down() { return m_down; }
    bool pressed_this_frame() { return m_pressed; }

    void on_update();

    void on_input_pressed(InputCode code);
    void on_input_released(InputCode code);

    void notify_pressed();
    void notify_released();

private:
    std::string m_name;

    bool m_input_is_mapped(InputCode code);

    std::vector<InputCode> m_inputs;
    int m_inputs_down_count;

    // down = button currently held down
    // pressed = button pressed down this frame/update
    bool m_down;
    bool m_pressed;

    std::vector<ButtonCallback> m_pressed_callbacks;
    std::vector<ButtonCallback> m_released_callbacks;
}