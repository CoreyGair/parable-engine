#include "Input/Control.h"

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

void Axis1DControl::on_update()
{
    // update cached values
    m_changed_this_frame = m_axis->dirty();
    m_cached_value = m_axis->get_value();

    // notify callbacks if was pressed/released in the frame
    if (m_changed_this_frame) notify_changed();

    // reset state
    m_axis->reset();
}
void Axis1DControl::on_event(Event* e)
{
    m_axis->on_event(e);
}

void Axis2DControl::on_update()
{
    // update cached values
    m_changed_this_frame = m_xaxis->dirty() |  m_yaxis->dirty();
    m_cached_value = glm::vec2(m_xaxis->get_value(), m_yaxis->get_value());

    // notify callbacks if was pressed/released in the frame
    if (m_changed_this_frame) notify_changed();

    // reset state
    m_xaxis->reset();
    m_yaxis->reset();
}
void Axis2DControl::on_event(Event* e)
{
    m_xaxis->on_event(e);
    m_yaxis->on_event(e);
}

void Axis3DControl::on_update()
{
    // update cached values
    m_changed_this_frame = m_xaxis->dirty() |  m_yaxis->dirty() | m_zaxis->dirty();
    m_cached_value = glm::vec3(m_xaxis->get_value(), m_yaxis->get_value(), m_zaxis->get_value());

    // notify callbacks if was pressed/released in the frame
    if (m_changed_this_frame) notify_changed();

    // reset state
    m_xaxis->reset();
    m_yaxis->reset();
    m_zaxis->reset();
}
void Axis3DControl::on_event(Event* e)
{
    m_xaxis->on_event(e);
    m_yaxis->on_event(e);
    m_zaxis->on_event(e);
}


}