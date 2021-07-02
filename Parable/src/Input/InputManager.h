#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"

#include <glm/glm.hpp>

namespace Parable::Input
{

// define flags for button state
#define BTN_DOWN BIT(0)     // key is currently down
#define BTN_PRESSED BIT(1)  // key was pressed this frame
using ButtonState = unsigned short int;

class InputManager
{
public:
    void on_event(EventDispatcher& dispatcher);
private:
    bool on_keyboard_pressed(KeyPressedEvent& e);
    bool on_keyboard_released(KeyReleasedEvent& e);
    bool on_mousebtn_pressed(MouseBtnPressedEvent& e);
    bool on_mousebtn_released(MouseBtnReleasedEvent& e);
    bool on_mouse_moved(MouseMovedEvent& e);
    bool on_mouse_scrolled(MouseScrolledEvent& e);

    std::map<KeyCode,ButtonState> m_key_states;
    std::map<MouseBtnCode,ButtonState> m_mousebtn_states;

    glm::vec2 m_mouse_delta;
    int m_scroll_delta;
};


}