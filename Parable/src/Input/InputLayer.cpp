#include "InputLayer.h"


namespace Parable::Input
{


void InputLayer::on_update()
{
    m_input_manager.on_update();
}

void InputLayer::on_event(Event* e)
{
    m_input_manager.on_event(e);
}


}