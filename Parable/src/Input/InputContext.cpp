#include "Input/InputContext.h"

namespace Parable::Input
{


void InputContext::on_update()
{
    for(auto& button_map : m_button_maps)
    {
        button_map.on_update();
    }
}

void InputContext::on_input_pressed(InputCode code)
{
    for(auto& button_map : m_button_maps)
    {
        button_map.on_input_pressed(code);
    }
}
void InputContext::on_input_released(InputCode code)
{
    for(auto& button_map : m_button_maps)
    {
        button_map.on_input_released(code);
    }
}


}