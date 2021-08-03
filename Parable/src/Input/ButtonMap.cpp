#include "ButtonMap.h"


namespace Parable::Input
{


bool ButtonMap::m_input_is_mapped(InputCode code)
{
    return std::find(m_inputs.cbegin(), m_inputs.cend(), code) != m_inputs.cend();
}


void ButtonMap::on_update()
{
    m_pressed = false;
}

/**
 * Updates state when an input button is pressed.
 * 
 * Sets pressed and down if the input is mapped, as well as calling the pressed callbacks.
 * 
 * @param code the input code of the button that was pressed
 */
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
/**
 * Updates state when an input button is released.
 * 
 * Resets down if the input is mapped, as well as calling the released callbacks.
 * 
 * @param code the input code of the button that was released
 */
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


}