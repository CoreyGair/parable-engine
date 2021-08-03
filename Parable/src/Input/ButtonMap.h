#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"

namespace Parable::Input
{


using ButtonCallback = std::function<void(void)>;

/**
 * Maps button inputs to a single 'vitrual button'.
 * 
 * Gets key and mouse button events, then uses them to update the state of a virtual button.
 * Managed by an InputContext, which passes it the relevant events.
 * Holds internal state for polling, as well as exposes callbacks for event-driven input.
 * 
 */
class ButtonMap{
public:
    ButtonMap(const std::string& name, std::vector<InputCode>& inputs) : m_name(name), m_inputs(std::move(inputs)) {}
    ButtonMap(ButtonMap&& other) : ButtonMap(other.m_name, other.m_inputs) {}
    
    ButtonMap& operator=(ButtonMap&& other)
    {
        m_name = std::move(other.m_name);
        m_inputs = std::move(other.m_inputs);
        return *this;
    }    

    const std::string& get_name() { return m_name; }

    bool is_down() { return m_down; }
    bool pressed_this_frame() { return m_pressed; }

    void on_update();

    void on_input_pressed(InputCode code);
    void on_input_released(InputCode code);

    void notify_pressed();
    void notify_released();

private:
    /**
     * Name of the virtual button.
     */
    std::string m_name;

    /**
     * Check if an input code is mapped by this map.
     */
    bool m_input_is_mapped(InputCode code);

    /**
     * List of the inputs which trigger this button.
     */
    std::vector<InputCode> m_inputs;
    /**
     * How many of the inputs are currently down.
     */
    int m_inputs_down_count = 0;

    /**
     * Is the button currently held down.
     */
    bool m_down = false;
    /**
     * Was the button pressed this frame.
     */
    bool m_pressed = false;

    /**
     * Callbacks to be called when the button is pressed.
     */
    std::vector<ButtonCallback> m_pressed_callbacks;
    /**
     * Callbacks to be called when the button is released.
     */
    std::vector<ButtonCallback> m_released_callbacks;
};


}