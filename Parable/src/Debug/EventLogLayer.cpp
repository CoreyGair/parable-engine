#include "Debug/EventLogLayer.h"

#include "Core/Log.h"

namespace Parable
{


void EventLogLayer::on_event(Event* e)
{
    if(e->get_event_category() & m_event_category_blacklist) return;
    PBL_CORE_INFO(e->to_string());
    e->handled |= true;
}


}