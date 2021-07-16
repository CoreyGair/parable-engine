#pragma once

namespace Parable::Input
{


template<class return_t>
void Axis1DControlBase<return_t>::on_update()
{
    // update cached values
    m_changed_this_frame = m_axis->dirty();
    m_cached_value = m_axis->get_value();

    // notify callbacks if was pressed/released in the frame
    if (m_changed_this_frame) notify_changed();

    // reset state
    m_axis->reset();
}
template<class return_t>
void Axis1DControlBase<return_t>::on_event(Event* e)
{
    m_axis->on_event(e);
}

template<class return_t, class axis_t>
void Axis2DControlBase<return_t, axis_t>::on_update()
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
template<class return_t, class axis_t>
void Axis2DControlBase<return_t, axis_t>::on_event(Event* e)
{
    m_xaxis->on_event(e);
    m_yaxis->on_event(e);
}

template<class return_t, class axis_t>
void Axis3DControlBase<return_t, axis_t>::on_update()
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
template<class return_t, class axis_t>
void Axis3DControlBase<return_t, axis_t>::on_event(Event* e)
{
    m_xaxis->on_event(e);
    m_yaxis->on_event(e);
    m_zaxis->on_event(e);
}


}