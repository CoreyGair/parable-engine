#include "InputManager.h"


namespace Parable::Input
{


void InputManager::on_event(EventDispatcher& dispatcher)
{
    if (dispatcher.event_in_category(EventCategoryButton))
    {
        if (dispatcher.dispatch<KeyPressedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::on_keyboard_pressed))) return;
        if (dispatcher.dispatch<KeyReleasedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::on_keyboard_released))) return;
        if (dispatcher.dispatch<MouseBtnPressedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::on_mousebtn_pressed))) return;
        if (dispatcher.dispatch<MouseBtnReleasedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::on_mousebtn_released))) return;
        if (dispatcher.dispatch<MouseMovedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::on_mouse_moved))) return;
        if (dispatcher.dispatch<MouseScrolledEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::on_mouse_scrolled))) return;
    }
}

bool InputManager::on_keyboard_pressed(KeyPressedEvent& e)
{
    if(m_key_states.count(e.get_key_code())!=0) m_key_states.emplace(e.get_key_code(), BTN_DOWN | BTN_PRESSED);
    m_key_states[e.get_key_code()] = BTN_DOWN | BTN_PRESSED;
    return true;
}
bool InputManager::on_keyboard_released(KeyReleasedEvent& e)
{
    if(m_key_states.count(e.get_key_code())!=0) m_key_states.emplace(e.get_key_code(), 0);
    m_key_states[e.get_key_code()] &= ~BTN_DOWN;
    return true;
}
bool InputManager::on_mousebtn_pressed(MouseBtnPressedEvent& e)
{
    if(m_key_states.count(e.get_button())!=0) m_key_states.emplace(e.get_button(), BTN_DOWN | BTN_PRESSED);
    m_key_states[e.get_button()] = BTN_DOWN | BTN_PRESSED;
    return true;
}
bool InputManager::on_mousebtn_released(MouseBtnReleasedEvent& e)
{
    if(m_key_states.count(e.get_button())!=0) m_key_states.emplace(e.get_button(), 0);
    m_key_states[e.get_button()] &= ~BTN_DOWN;
    return true;
}
bool InputManager::on_mouse_moved(MouseMovedEvent& e)
{
    m_mouse_delta += glm::vec2(e.get_x(), e.get_y());
    return true;
}
bool InputManager::on_mouse_scrolled(MouseScrolledEvent& e)
{
    m_scroll_delta += e.get_scrol_amt();
    return true;
}


}