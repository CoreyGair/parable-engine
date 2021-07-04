#include "InputManager.h"


namespace Parable::Input
{


InputManager::InputManager()
{
    PBL_ASSERT_MSG(!s_instance, "Input manager already exists!");
    s_instance = this;
}

void InputManager::on_update()
{
    for(auto it = m_contexts.begin(); it != m_contexts.end(); ++it)
    {
        if ((*it).enabled)
        {
            (*it).on_update();
        }
    }
}

void InputManager::on_event(Event* e)
{
    for(auto it = m_contexts.begin(); it != m_contexts.end(); ++it)
    {
        if ((*it).enabled)
        {
            (*it).on_event(e);
        }
    }
}


}