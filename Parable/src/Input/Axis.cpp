#include "Input/Axis.h"

namespace Parable::Input
{


void MouseXAxis::on_event(Event* e)
{
    EventDispatcher dispatcher (e);
    dispatcher.dispatch<MouseMovedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(MouseXAxis::event_to_value));
}
bool MouseXAxis::event_to_value(MouseMovedEvent& e)
{
    m_value += e.get_x();
    m_dirty = true;
    return true;
}

void MouseYAxis::on_event(Event* e)
{
    EventDispatcher dispatcher (e);
    dispatcher.dispatch<MouseMovedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(MouseYAxis::event_to_value));
}
bool MouseYAxis::event_to_value(MouseMovedEvent& e)
{
    m_value += e.get_y();
    m_dirty = true;
    return true;
}

void MouseScrollAxis::on_event(Event* e)
{
    EventDispatcher dispatcher (e);
    dispatcher.dispatch<MouseScrolledEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(MouseScrollAxis::event_to_value));
}
bool MouseScrollAxis::event_to_value(MouseScrolledEvent& e)
{
    m_value += e.get_scroll_amt();
    m_dirty = true;
    return true;
}

void ButtonAxis::on_event(Event* e)
{
    EventDispatcher dispatcher (e);
    if(dispatcher.dispatch<KeyPressedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(ButtonAxis::handle_key_pressed))) return;
    if(dispatcher.dispatch<KeyReleasedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(ButtonAxis::handle_key_released))) return;
    if(dispatcher.dispatch<MouseBtnPressedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(ButtonAxis::handle_mousebtn_pressed))) return;
    if(dispatcher.dispatch<MouseBtnReleasedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(ButtonAxis::handle_mousebtn_released))) return;
}
bool ButtonAxis::handle_button_pressed(InputCode c)
{
    m_dirty = true;
    if (c == m_positive_input)
    {
        if (m_value <= 1) m_value += 1;
    }
    else if (c == m_negative_input)
    {
        if (m_value >= -1) m_value -= 1;
    }
    else return false;
    return true;
}
bool ButtonAxis::handle_button_released(InputCode c)
{
    m_dirty = true;
    if (c == m_positive_input)
    {
        if (m_value >= 0) m_value -= 1;
    }
    else if (c == m_negative_input)
    {
        if (m_value <= 0) m_value += 1;
    }
    else return false;
    return true;
}
bool ButtonAxis::handle_key_pressed(KeyPressedEvent& e) { return handle_button_pressed(e.get_key_code()); }
bool ButtonAxis::handle_mousebtn_pressed(MouseBtnPressedEvent& e) { return handle_button_pressed(e.get_button()); }
bool ButtonAxis::handle_key_released(KeyReleasedEvent& e) { return handle_button_released(e.get_key_code()); }
bool ButtonAxis::handle_mousebtn_released(MouseBtnReleasedEvent& e) { return handle_button_released(e.get_button()); }


}