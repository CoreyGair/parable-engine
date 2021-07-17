#include "ButtonMap.h"


bool ButtonMap::m_input_is_mapped(InputCode code)
{
    return std::find(m_inputs.cbegin(), m_inputs.cend(), code) == m_inputs.cend();
}


void on_update()
{
    m_pressed = false;
}


void ButtonMap::on_input_pressed(InputCode code)
{
    if (m_input_is_mapped(code))
    {
        ++m_inputs_down_count;
        if (!m_down)
        {
            m_down = true;
            m_pressed = true;

            notify_pressed();
        }
    }
}
void ButtonMap::on_input_released(InputCode code)
{
    if (m_input_is_mapped(code))
    {
        --m_inputs_down_count;
        if (m_inputs_down_count == 0)
        {
            m_down = false;

            notify_released();
        }
    }
}

void ButtonMap::notify_pressed()
{
    for(auto cb : m_pressed_callbacks)
    {
        cb();
    }
}
void ButtonMap::notify_released()
{
    for(auto cb : m_released_callbacks)
    {
        cb();
    }
}