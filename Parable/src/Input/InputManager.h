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

    bool is_key_down(KeyCode key) { return m_input_state.key_down[key]; }
    bool is_key_pressed(KeyCode key) { return m_input_state.key_pressed[key]; }


private:
    std::vector<InputContext> m_contexts;

    bool key_pressed(KeyPressedEvent& e);
    bool key_released(KeyReleasedEvent& e);
    bool mouse_btn_pressed(MouseBtnPressedEvent& e);
    bool mouse_btn_released(MouseBtnReleasedEvent& e);
    bool mouse_scrolled(MouseScrolledEvent& e);
    bool mouse_moved(MouseMovedEvent& e);

    InputState m_input_state;
    
};

struct InputState
{
    // down refers to the key/btn being held down, pressed means it became down after the previous on_update
    std::bitset<KeyCode::LAST-KeyCode::FIRST> key_down;
    std::bitset<KeyCode::LAST-KeyCode::FIRST> key_pressed;
    std::bitset<MouseButton::LAST-MouseButton::FIRST> mouse_btn_down;
    std::bitset<MouseButton::LAST-MouseButton::FIRST> mouse_btn_pressed;
    glm::vec2 mouse_position;
    glm::vec2 mouse_position_delta;
    float mouse_scroll_delta;
}


}