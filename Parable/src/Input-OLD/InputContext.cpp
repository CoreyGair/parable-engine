#include "Input/InputContext.h"

namespace Parable::Input
{


void InputContext::on_update()
{
    for(auto it = m_controls.begin(); it != m_controls.end(); ++it)
    {
        (*it)->on_update();
    }
}
void InputContext::on_event(Event* e)
{
    for(auto it = m_controls.begin(); it != m_controls.end(); ++it)
    {
        (*it)->on_event(e);
    }
}


}