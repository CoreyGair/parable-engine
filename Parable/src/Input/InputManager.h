#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"
#include "Input/InputContext.h"

#include <glm/glm.hpp>

namespace Parable::Input
{


struct InputState
{
    // down refers to the key/btn being held down, pressed means it became down after the previous on_update
    std::bitset<(int)KeyCode::LAST-(int)KeyCode::FIRST> key_down;
    std::bitset<(int)KeyCode::LAST-(int)KeyCode::FIRST> key_pressed;
    std::bitset<(int)MouseButton::LAST-(int)MouseButton::FIRST> mouse_btn_down;
    std::bitset<(int)MouseButton::LAST-(int)MouseButton::FIRST> mouse_btn_pressed;
    glm::vec2 mouse_position = glm::vec2(0,0);
    glm::vec2 mouse_position_delta = glm::vec2(0,0);
    float mouse_scroll_delta = 0;
};


class InputManager
{
public:
    static InputManager* s_instance; // singleton
    static InputManager& get_instance() { return *s_instance; }

    InputManager();
    ~InputManager() { s_instance = nullptr; }

    void on_update();
    void on_event(Event* e);

    bool is_key_down(KeyCode key) { return m_input_state.key_down[(int)key]; }
    bool is_key_pressed(KeyCode key) { return m_input_state.key_pressed[(int)key]; }

    bool is_mouse_btn_down(MouseButton btn) { return m_input_state.mouse_btn_down[(int)btn]; }
    bool is_mouse_btn_pressed(MouseButton btn) { return m_input_state.mouse_btn_pressed[(int)btn]; }

    glm::vec2 get_mouse_position() { return m_input_state.mouse_position; }
    glm::vec2 get_mouse_delta() { return m_input_state.mouse_position_delta; }
    float get_scroll_amt() { return m_input_state.mouse_scroll_delta; }

private:
    std::vector<InputContext> m_contexts;

    bool key_pressed(KeyPressedEvent& e);
    bool key_released(KeyReleasedEvent& e);
    bool mouse_btn_pressed(MouseBtnPressedEvent& e);
    bool mouse_btn_released(MouseBtnReleasedEvent& e);
    bool mouse_scrolled(MouseScrolledEvent& e);
    bool mouse_moved(MouseMovedEvent& e);

    void notify_contexts_of_input_pressed(InputCode code);
    void notify_contexts_of_input_released(InputCode code);

    InputState m_input_state;
    
};

}