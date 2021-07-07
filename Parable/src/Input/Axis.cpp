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
    if (is_positive_input(c))
    {
        if (m_positive_inputs.size() > m_num_positive_down) ++m_num_positive_down;
    }
    else if (is_negative_input(c))
    {
        if (m_negative_inputs.size() > m_num_negative_down) ++m_num_negative_down;
    }
    else return false;
    m_dirty = true;
    return true;
}
bool ButtonAxis::handle_button_released(InputCode c)
{
    if (is_positive_input(c))
    {
        if (m_num_positive_down > 0) --m_num_positive_down;
    }
    else if (is_negative_input(c))
    {
        if (m_num_negative_down > 0) --m_num_negative_down;
    }
    else return false;
    m_dirty = true;
    return true;
}
bool ButtonAxis::handle_key_pressed(KeyPressedEvent& e) { return handle_button_pressed(e.get_key_code()); }
bool ButtonAxis::handle_mousebtn_pressed(MouseBtnPressedEvent& e) { return handle_button_pressed(e.get_button()); }
bool ButtonAxis::handle_key_released(KeyReleasedEvent& e) { return handle_button_released(e.get_key_code()); }
bool ButtonAxis::handle_mousebtn_released(MouseBtnReleasedEvent& e) { return handle_button_released(e.get_button()); }

bool ButtonAxis::is_positive_input(InputCode c) { return std::find(m_positive_inputs.cbegin(), m_positive_inputs.cend(), c) != m_positive_inputs.cend(); }
bool ButtonAxis::is_negative_input(InputCode c) { return std::find(m_negative_inputs.cbegin(), m_negative_inputs.cend(), c) != m_negative_inputs.cend(); }

void ButtonAxis::update_value() { m_value = (m_num_positive_down > 0 ? 1 : 0) - (m_num_negative_down > 0 ? 1 : 0); }


}