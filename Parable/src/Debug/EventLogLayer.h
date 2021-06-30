#pragma once

#include "Core/Base.h"
#include "Core/Layer.h"
#include "Events/Event.h"

namespace Parable
{

// a simple debug layer for logging events
class EventLogLayer : public Layer
{
private:
    std::string m_name;

    // which event categories should we ignore (if any)
    int m_event_category_blacklist;

    bool log_event(Event* e);

public:
    EventLogLayer(int event_category_blacklist = 0) : Layer(std::string("EventLogLayer")), m_event_category_blacklist(event_category_blacklist) {}

    void on_event(EventDispatcher& dispatcher) override;
};


}