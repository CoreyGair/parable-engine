#include "Control.h"

namespace Parable::Input
{


void ButtonControl::on_update()
{
    // update cached values
    bool prev_value = m_cached_value;
    m_cached_value = get_value() == 0;

    m_pressed_this_frame = false;
    m_released_this_frame = false;
    if (prev_value != m_cached_value)
    {
        if (prev_value) m_released_this_frame = true;
        else m_pressed_this_frame = true;
    }

    // notify callbacks if was pressed/released in the frame
    if (m_pressed_this_frame) notify_pressed();
    if (m_pressed_this_frame) notify_released();
}
void ButtonControl::on_event(Event* e)
{
    m_button_axis.on_event(e); 
}


}