#include "Debug/EventLogLayer.h"

#include "Core/Log.h"

namespace Parable
{


void EventLogLayer::on_event(EventDispatcher& dispatcher)
{
    dispatcher.dispatch_raw(PBL_BIND_MEMBER_EVENT_HANDLER(EventLogLayer::log_event));
}

bool EventLogLayer::log_event(Event* e)
{
    PBL_CORE_INFO(e->to_string());
    return false;
}


}